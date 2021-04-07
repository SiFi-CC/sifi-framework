// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersDDUnpacker.h"
#include "SCategory.h"
#include "SDDSamples.h"
#include "SDatabase.h"
#include "SFibersDDCalibratorPar.h"
#include "SFibersDDUnpackerPar.h"
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SiFi.h"

#include <algorithm>
#include <iostream>
#include <numeric>

/**
 * \class SFibersDDUnpacker
\ingroup lib_fibers

A unpacker task.

\sa STask
*/

Float_t FindT0(Float_t* samples, size_t len, Float_t threshold, Int_t pol)
{
    Float_t t0 = 0.;
    Int_t istop = -1;

    for (uint i = 0; i < len; ++i)
    {
        if ((pol == 0 and samples[i] < threshold) or (pol == 1 and samples[i] > threshold))
        {
            istop = i;
            if (i == 0) t0 = 0.;
            break;
        }
        else
        {
            t0 = -100.;
            istop = i;
        }
    }

    if (istop != 0 and istop != len - 1)
    {
        t0 = (istop - 1) + (threshold - samples[istop - 1]) / (samples[istop] - samples[istop - 1]);
    }

    return t0;
}

Float_t FindTMax(Float_t* samples, size_t len, Float_t threshold, Int_t _t0, Int_t pol,
                 Int_t deadtime, Int_t& pileup)
{

    if (_t0 == -100) return -100.;

    Float_t tmax = -1.;
    Int_t wait_for_pileup = 0;

    for (Int_t ii = _t0; ii < len; ii++)
    {
        if (tmax == -1. and
            ((pol == 0 and samples[ii] > threshold) or (pol == 1 and samples[ii] < threshold)))
        {
            tmax = ii - 1 + (threshold - samples[ii - 1]) / (samples[ii] - samples[ii - 1]);
            wait_for_pileup = 1;
        }

        if (wait_for_pileup and (ii < _t0 + deadtime) and
            ((pol == 0 and samples[ii] < threshold) or (pol == 1 and samples[ii] > threshold)))
        {
            pileup = 1;
            break;
        }
    }

    if (fabs(tmax) < 1E-8) { tmax = len - 1; }
    return tmax;
}

Int_t FindVeto(Float_t* samples, size_t limit, Float_t threshold, Int_t pol)
{

    if (pol == 0)
    {
        for (int i = 0; i < limit; i++)
        {
            if (samples[i] > threshold) return 1;
        }
    }
    else if (pol == 1)
    {
        for (int i = 0; i < limit; i++)
        {
            if (samples[i] < threshold) return 1;
        }
    }

    return 0;
}

/**
 * Constructor
 */
SFibersDDUnpacker::SFibersDDUnpacker()
    : SDDUnpacker(), catDDSamples(nullptr), catFibersRaw(nullptr), pDDUnpackerPar(nullptr),
      pLookUp(nullptr)
{
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersDDUnpacker::init()
{
    SDDUnpacker::init();

    size_t sim_sizes[3];
    sim_sizes[0] = 1;
    sim_sizes[1] = 1;
    sim_sizes[2] = 16;

    if (!sifi()->registerCategory(SCategory::CatDDSamples, "SDDSamples", 3, sim_sizes, false))
        return false;

    catDDSamples = sifi()->buildCategory(SCategory::CatDDSamples);
    if (!catDDSamples)
    {
        std::cerr << "No CatDDSamples category" << std::endl;
        return false;
    }

    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersRaw category" << std::endl;
        return false;
    }

    // get calibrator parameters
    pDDUnpackerPar =
        dynamic_cast<SFibersDDUnpackerPar*>(pm()->getParContainer("FibersDDUnpackerPar"));
    if (!pDDUnpackerPar)
    {
        std::cerr << "Parameter container 'SFibersDDUnpackerPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }
    pDDUnpackerPar->print();

    // get calibrator parameters
    pDDCalPar =
        dynamic_cast<SFibersDDCalibratorPar*>(pm()->getCalContainer("FibersDDCalibratorPar"));
    if (!pDDCalPar)
    {
        std::cerr << "Parameter container 'SFibersDDCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersDDLookupTable"));
    pLookUp->print();

    return true;
}

/**
 * Decode data from the source. The source calls decode function passing the
 * subevent id, buffer with float numbers amd length of the buffer.
 *
 * \param subevtid subevent id of the data
 * \param data data buffer
 * \param length of the data
 * \return reading of the data was successful
 */
bool SFibersDDUnpacker::decode(uint16_t subevtid, float* data, size_t length)
{
    uint16_t fake_address = subevtid & 0xfff0;
    uint8_t channel = subevtid & 0x0f;

    Float_t thr = pDDUnpackerPar->getThreshold(channel);
    Int_t pol = pDDUnpackerPar->getPolarity();
    Int_t anamode = pDDUnpackerPar->getAnaMode();
    Int_t intmode = pDDUnpackerPar->getIntMode();
    Int_t deadtime = pDDUnpackerPar->getDeadTime();
    Int_t blmode = pDDUnpackerPar->getBLMode(channel);
    Float_t veto_thr = pDDUnpackerPar->getVetoThreshold(channel);

    SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(fake_address, channel));
    SLocator loc(3);
    loc[0] = lc->m; // mod;
    loc[1] = lc->l; // lay;
    loc[2] = lc->s; // fib;
    char side = lc->side;

    auto cp = *pDDCalPar->getPar(lc);

    SDDSamples* pSamples = dynamic_cast<SDDSamples*>(catDDSamples->getObject(loc));
    if (!pSamples)
    {
        pSamples = reinterpret_cast<SDDSamples*>(catDDSamples->getSlot(loc));
        new (pSamples) SDDSamples;
        pSamples->Clear();
    }

    pSamples->setAddress(loc[0], loc[1], loc[2]);

    // Float_t samples[1024];
    // size_t limit = length <= 1024 ? length : 1024;
    Float_t samples[length];
    size_t limit = length;

    memcpy(samples, data, limit * sizeof(float));

    // find baseline
    Float_t bl = -1;
    Float_t bl_sigma = 0;
    Int_t samples_for_bl = 50;

    if (blmode == 0)
    {
        bl = std::accumulate(samples, samples + samples_for_bl, 0.);
        bl /= samples_for_bl;
        bl_sigma = 0;
        for (int i = 0; i < samples_for_bl; ++i)
        {
            bl_sigma += (bl - samples[i]) * (bl - samples[i]);
        }
        bl_sigma = sqrt(bl_sigma / samples_for_bl);
    }
    else
    {
        bl = blmode;
    }

    std::transform(samples, samples + length, samples, [bl](float f) { return f - bl; });

    Float_t threshold = 0;
    Float_t ampl = -100;
    Int_t pileup = 0;
    Float_t tot = -100;
    Float_t charge = -100;

    if (pol == 1)
    {
        ampl = *std::max_element(samples, samples + limit);
        threshold = anamode == 0 ? thr : thr / 100 * ampl;
    }
    else
    {
        ampl = -*std::min_element(samples, samples + limit);
        threshold = anamode == 0 ? thr : thr / 100 * ampl;
    }
    Float_t t0 = FindT0(samples, limit, threshold, pol);

    if (t0 >= 0)
    {
        Float_t _mod = t0 - int(t0);
        Int_t _t0 = _mod > 0. ? int(t0 + 1) : int(t0);

        Float_t tmax = FindTMax(samples, limit, threshold, _t0, pol, deadtime, pileup);
        tot = tmax - t0;

        Int_t _tmax = tmax;
        Int_t _len = intmode <= 0 ? _tmax - _t0 : intmode;

        if (_len + _t0 >= 1024) _len = 1024 - _t0 - 1;

        size_t offset_start = _t0 <= 20 ? 0 : _t0 - 20;
        size_t offset_stop = _len + _t0 >= 1024 ? 1024 - _t0 - 1 : _t0 + _len;

        charge = std::accumulate(samples + offset_start, samples + offset_stop,
                                 0.); // backward integration here
        if (pol == 0) charge = -charge;
    }

    Int_t veto = FindVeto(samples, limit, veto_thr, pol);

    SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(loc));
    if (!pRaw)
    {
        pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
        new (pRaw) SFibersRaw;
    }

    float ADC_to_mV = getADCTomV();
    float sample_to_ns = getSampleTimeBin();

    pRaw->setAddress(loc[0], loc[1], loc[2]);
    if (side == 'l')
    {
        float tmp_charge = charge / ADC_to_mV;
        if (save_samples) pSamples->fillSamplesL(data, length);
        pSamples->getSignalL()->SetAmplitude(ampl / ADC_to_mV);
        pSamples->getSignalL()->SetT0(t0 * sample_to_ns);
        pSamples->getSignalL()->SetTOT(tot * sample_to_ns);
        pSamples->getSignalL()->SetCharge(tmp_charge);
        pSamples->getSignalL()->SetBL(bl);
        pSamples->getSignalL()->SetBLSigma(bl_sigma);
        pSamples->getSignalL()->SetPileUp(pileup);
        pSamples->getSignalL()->SetVeto(veto);
        pSamples->getSignalL()->SetPE(tmp_charge / cp[0] + cp[1]);

        pRaw->setQDCL(tmp_charge);
        pRaw->setTimeL(t0);
    }
    if (side == 'r')
    {
        float tmp_charge = charge / ADC_to_mV;
        if (save_samples) pSamples->fillSamplesR(data, length);
        pSamples->getSignalR()->SetAmplitude(ampl / ADC_to_mV);
        pSamples->getSignalR()->SetT0(t0 * sample_to_ns);
        pSamples->getSignalR()->SetTOT(tot * sample_to_ns);
        pSamples->getSignalR()->SetCharge(tmp_charge);
        pSamples->getSignalR()->SetBL(bl);
        pSamples->getSignalR()->SetBLSigma(bl_sigma);
        pSamples->getSignalR()->SetPileUp(pileup);
        pSamples->getSignalR()->SetVeto(veto);
        pSamples->getSignalR()->SetPE(tmp_charge / cp[0] + cp[1]);

        pRaw->setQDCR(tmp_charge);
        pRaw->setTimeR(t0);
    }

    return true;
}
