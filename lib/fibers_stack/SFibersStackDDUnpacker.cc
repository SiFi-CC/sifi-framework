// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackDDUnpacker.h"
#include "SCategory.h"
#include "SDDSamples.h"
#include "SFibersStackDDUnpackerPar.h"
#include "SFibersStackRaw.h"
#include "SParManager.h"
#include "SiFi.h"

#include <algorithm>
#include <iostream>
#include <numeric>

const Float_t adc_to_mv = 4.096;
const Float_t sample_to_ns = 1.;
/** \class SFibersStackDDUnpacker
\ingroup lib_fibers_stack

A unpacker task.

\sa STask
*/

Float_t FindT0(Float_t* samples, size_t len, Float_t threshold, Int_t pol)
{
    Float_t t0 = 0.;
    Int_t istop = -1;

    for (uint i = 0; i < len; ++i)
    {
        if ((pol == 0 and samples[i] < threshold) or
            (pol == 1 and samples[i] > threshold))
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
        t0 = (istop - 1) + (threshold - samples[istop - 1]) /
                               (samples[istop] - samples[istop - 1]);
    }

    return t0;
}

Float_t FindTMax(Float_t* samples, size_t len, Float_t threshold, Int_t _t0,
                 Int_t pol, Int_t deadtime, Int_t& pileup)
{

    if (_t0 == -100) return -100.;

    Float_t tmax = -1.;
    Int_t wait_for_pileup = 0;

    for (Int_t ii = _t0; ii < len; ii++)
    {
        if (tmax == -1. and ((pol == 0 and samples[ii] > threshold) or
                             (pol == 1 and samples[ii] < threshold)))
        {
            tmax =
                ii - 1 +
                (threshold - samples[ii - 1]) / (samples[ii] - samples[ii - 1]);
            wait_for_pileup = 1;
        }

        if (wait_for_pileup and (ii < _t0 + deadtime) and
            ((pol == 0 and samples[ii] < threshold) or
             (pol == 1 and samples[ii] > threshold)))
        {
            pileup = 1;
            break;
        }
    }

    if (fabs(tmax) < 1E-8) { tmax = len - 1; }
    return tmax;
}

/** Constructor
 */
SFibersStackDDUnpacker::SFibersStackDDUnpacker(uint16_t address,
                                               uint8_t channel)
    : SDDUnpacker(address), channel(channel), catDDSamples(nullptr)
{
}

/** Destructor
 */
SFibersStackDDUnpacker::~SFibersStackDDUnpacker() {}

/** Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersStackDDUnpacker::init()
{
    SDDUnpacker::init();

    size_t sim_sizes[3];
    sim_sizes[0] = 1;
    sim_sizes[1] = 1;
    sim_sizes[2] = 16;

    if (!sifi()->registerCategory(SCategory::CatDDSamples, "SDDSamples", 3,
                                  sim_sizes, false))
        return false;

    catDDSamples = sifi()->buildCategory(SCategory::CatDDSamples);
    if (!catDDSamples)
    {
        std::cerr << "No CatDDSamples category"
                  << "\n";
        return false;
    }

    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersStackRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersStackRaw category"
                  << "\n";
        return false;
    }

    // get calibrator parameters
    pDDUnpackerPar = (SFibersStackDDUnpackerPar*)pm()->getParameterContainer(
        "SFibersStackDDUnpackerPar");
    if (!pDDUnpackerPar)
    {
        std::cerr << "Parameter container 'SFibersStackDDUnpackerPar' was not "
                     "obtained!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    pDDUnpackerPar->print();
    return true;
}

bool SFibersStackDDUnpacker::decode(float* data, size_t length)
{
    Float_t thr = pDDUnpackerPar->getThreshold(channel);
    Int_t pol = pDDUnpackerPar->getPolarity();
    Int_t anamode = pDDUnpackerPar->getAnaMode();
    Int_t intmode = pDDUnpackerPar->getIntMode();
    Int_t deadtime = pDDUnpackerPar->getDeadTime();

    SLocator loc(3);
    loc[0] = 0;       // mod;
    loc[1] = 0;       // lay;
    loc[2] = channel; // fib;

    SDDSamples* pSamples = (SDDSamples*)catDDSamples->getObject(loc);
    if (!pSamples)
    {
        pSamples = (SDDSamples*)catDDSamples->getSlot(loc);
        pSamples = new (pSamples) SDDSamples;
    }

    pSamples->setAddress(loc[0], loc[1], loc[2]);

    if (pSamples) pSamples->fillSamples(data, length);

    // copy samples
    Float_t samples[1024];
    size_t limit = length <= 1024 ? length : 1024;
    memcpy(samples, data, limit * sizeof(float));

    // find baseline
    Float_t bl = std::accumulate(samples, samples + 50, 0);
    bl /= 50;

    Float_t bl_sigma = 0;
    for (int i = 0; i < 50; ++i)
    {
        bl_sigma += (bl - samples[i]) * (bl - samples[i]);
    }
    bl_sigma = sqrt(bl_sigma / 50.);

    for (auto& s : samples)
        s -= bl;

    Float_t threshold = 0;
    Float_t ampl = 0;
    Int_t pileup = 0;

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

    Float_t _mod = t0 - int(t0);
    Int_t _t0 = _mod > 0. ? int(t0 + 1) : int(t0);

    Float_t tmax =
        FindTMax(samples, limit, threshold, _t0, pol, deadtime, pileup);
    Float_t tot = tmax - t0;

    Int_t _tmax = tmax;
    Int_t _len = intmode <= 0 ? _tmax - _t0 : intmode;
    Float_t charge = std::accumulate(samples + _t0, samples + _t0 + _len, 0);
    if (pol == 0) charge = -charge;

    pSamples->getSignal()->SetAmplitude(ampl / adc_to_mv);
    pSamples->getSignal()->SetT0(t0 /** sample_to_ns*/);
    pSamples->getSignal()->SetTOT(tot /** sample_to_ns*/);
    pSamples->getSignal()->SetCharge(charge / adc_to_mv);
    pSamples->getSignal()->fBL = bl;
    pSamples->getSignal()->fBL_sigma = bl_sigma;
    pSamples->getSignal()->fPileUp = pileup;

    SFibersStackRaw* pRaw = (SFibersStackRaw*)catFibersRaw->getObject(loc);
    if (!pRaw)
    {
        pRaw = (SFibersStackRaw*)catFibersRaw->getSlot(loc);
        pRaw = new (pRaw) SFibersStackRaw;
    }

    pRaw->setAddress(loc[0], loc[1], loc[2]);
    pRaw->setADC(charge / adc_to_mv);
    pRaw->setTime(t0);

    return true;
}
