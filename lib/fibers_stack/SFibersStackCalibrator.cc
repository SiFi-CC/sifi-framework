// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackCalibrator.h"
#include "SCalContainer.h"
#include "SCategory.h"
#include "SFibersStackCal.h"
#include "SFibersStackCalibratorPar.h"
#include "SFibersStackLookup.h"
#include "SFibersStackRaw.h"
#include "SLocator.h"
#include "SParManager.h"
#include "SiFi.h"

#include "RtypesCore.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>

/**
 * \class SFibersStackCalibrator
\ingroup lib_fibers_stack

A calibraror task for Fibers Stack.

Takes MGeantFibersStackRaw data and applies calibration. See MTask for the
interface description.

*/

/**
 * Default constructor
 */
SFibersStackCalibrator::SFibersStackCalibrator()
    : STask(), catFibersRaw(nullptr), catFibersCal(nullptr), pCalibratorPar(nullptr)
{
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersStackCalibrator::init()
{
    // get Raw category
    catFibersRaw = sifi()->getCategory(SCategory::CatFibersStackRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersStackRaw category" << std::endl;
        return false;
    }

    // create Cal category
    catFibersCal = sifi()->buildCategory(SCategory::CatFibersStackCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersStackCal category" << std::endl;
        return false;
    }

    // get calibrator parameters
    pCalibratorPar =
        dynamic_cast<SCalContainer<3>*>(pm()->getCalibrationContainer("SFibersStackCalibratorPar"));
    if (!pCalibratorPar)
    {
        std::cerr << "Parameter container 'SFibersStackCalibratorPar' was not obtained!"
                  << std::endl;
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
bool SFibersStackCalibrator::execute()
{
    int size = catFibersRaw->getEntries();

    for (int i = 0; i < size; ++i)
    {
        SFibersStackRaw* pRaw = dynamic_cast<SFibersStackRaw*>(catFibersRaw->getObject(i));
        if (!pRaw)
        {
            printf("FibersStackRaw doesn't exists!\n");
            continue;
        }

        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pRaw->getAddress(mod, lay, fib);

        SFibersStackChannel chan_l;
        chan_l.m = mod;
        chan_l.l = lay;
        chan_l.s = fib;
        chan_l.side = 'l';
        SFibersStackChannel chan_r = chan_l;
        chan_r.side = 'r';

        SCalPar<3>* cp_l = pCalibratorPar->getPar(&chan_l);
        SCalPar<3>* cp_r = pCalibratorPar->getPar(&chan_r);

        // calc laboratory coordinates from digi data
        Float_t qdc_l = pRaw->getQDCL();
        Float_t qdc_r = pRaw->getQDCR();
        Float_t time_l = pRaw->getTimeL();
        Float_t time_r = pRaw->getTimeR();

        // do your magic here with u, y and qdc
        Float_t energy_l = cp_l->par[0] * qdc_l + cp_l->par[1];
        Float_t energy_r = cp_r->par[0] * qdc_r + cp_r->par[1];
        time_l += cp_l->par[2];
        time_r += cp_r->par[2];

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackCal* pCal = dynamic_cast<SFibersStackCal*>(catFibersCal->getObject(loc));
        if (!pCal)
        {
            pCal = reinterpret_cast<SFibersStackCal*>(catFibersCal->getSlot(loc));
            new (pCal) SFibersStackCal;
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
bool SFibersStackCalibrator::finalize() { return true; }
