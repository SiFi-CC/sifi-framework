// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersCalibrator.h"
#include "SCalContainer.h"
#include "SCategory.h"
#include "SDatabase.h"
#include "SFibersCal.h"
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SLocator.h"
#include "SiFi.h"

#include <RtypesCore.h>
#include <TObject.h> // for TObject

#include <cmath> // for pow, exp
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>

/**
 * \class SFibersCalibrator
\ingroup lib_fibers

A calibraror task for Fibers Stack.

Takes MGeantFibersRaw data and applies calibration. See MTask for the
interface description.

*/

/**
 * Default constructor
 */
SFibersCalibrator::SFibersCalibrator()
    : STask(), catFibersRaw(nullptr), catFibersCal(nullptr), pCalibratorPar(nullptr)
{
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersCalibrator::init()
{
    // get Raw category
    catFibersRaw = sifi()->getCategory(SCategory::CatFibersRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersRaw category" << std::endl;
        return false;
    }

    // create Cal category
    catFibersCal = sifi()->buildCategory(SCategory::CatFibersCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersCal category" << std::endl;
        return false;
    }

    // get calibrator parameters
    pCalibratorPar =
        dynamic_cast<SCalContainer<6>*>(rdb()->getCalContainer("FibersCalibratorPar"));

    if (!pCalibratorPar)
    {
        std::cerr << "Parameter container 'SFibersCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

/**
 * Execute task
 *
 * \sa STask::execute()
 * \return success
 */
bool SFibersCalibrator::execute()
{
    int size = catFibersRaw->getEntries();

    for (int i = 0; i < size; ++i)
    {
        SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(i));
        if (!pRaw)
        {
            printf("FibersRaw doesn't exists!\n");
            continue;
        }

        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pRaw->getAddress(mod, lay, fib);

        SFibersChannel chan_l;
        chan_l.m = mod;
        chan_l.l = lay;
        chan_l.s = fib;
        chan_l.side = 'l';
        SFibersChannel chan_r = chan_l;
        chan_r.side = 'r';

        auto _cp_l = pCalibratorPar->getPar(&chan_l);
        auto _cp_r = pCalibratorPar->getPar(&chan_r);

        if (!_cp_l or !_cp_r)
        {
            if (!_cp_l)
                printf("Calibration parametes for ml,f,s = %d,%d,%d,%c does not exists\n", mod, lay,
                       fib, 'l');
            if (!_cp_r)
                printf("Calibration parametes for ml,f,s = %d,%d,%d,%c does not exists\n", mod, lay,
                       fib, 'r');
            abort();
        }

        auto&& cp_l = *_cp_l;
        auto&& cp_r = *_cp_r;

        // parameters in cp_l/cp_r:
        // [0] - lambda
        // [1] - eta_r
        // [2] - eta_l
        // [3] - ksi
        // [4] - length
        // [5] - time offset

        // calc laboratory coordinates from digi data
        Float_t qdc_l = pRaw->getQDCL();
        Float_t qdc_r = pRaw->getQDCR();
        Float_t time_l = pRaw->getTimeL();
        Float_t time_r = pRaw->getTimeR();

        // calibration: measured signal -> direct signal
        Float_t e = exp(cp_l[0] / cp_l[4]);
        Float_t energy_l = (e * (e * cp_l[3] * qdc_l - (qdc_r * cp_l[1]))) /
                           (cp_l[3] * (pow(e, 2) - (cp_l[1] * cp_l[2])));
        Float_t energy_r = -(e * (-e * qdc_r + (cp_r[3] * qdc_l * cp_r[2]))) /
                           (cp_r[3] * (pow(e, 2) - (cp_r[1] * cp_r[2])));

        time_l += cp_l[5];
        time_r += cp_r[5];

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersCal* pCal = dynamic_cast<SFibersCal*>(catFibersCal->getObject(loc));
        if (!pCal)
        {
            pCal = reinterpret_cast<SFibersCal*>(catFibersCal->getSlot(loc));
            new (pCal) SFibersCal;
            pCal->Clear();
        }

        pCal->setAddress(mod, lay, fib);
        pCal->setQDC(energy_l, energy_r);
        pCal->setTime(time_l, time_r);
    }

    return true;
}

/**
 * Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SFibersCalibrator::finalize() { return true; }
