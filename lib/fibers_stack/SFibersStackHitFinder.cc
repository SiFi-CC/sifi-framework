// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SCategory.h"
#include "SFibersStackCal.h"
#include "SFibersStackHit.h"
#include "SFibersStackHitFinder.h"
#include "SFibersStackHitFinderPar.h"
#include "SLocator.h"
#include "SiFi.h"

#include <RtypesCore.h>

#include <cstdio>
#include <math.h> 
#include <iostream>

/**
 * \class SFibersStackHitFinder
\ingroup lib_fibers_stack

A hit finder task for Fibers Stack.

Takes each fiber data and tries to reconstruct hit along the fiber.

*/

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersStackHitFinder::init()
{

    // get Cal category
    catFibersCal = sifi()->getCategory(SCategory::CatFibersStackCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersStackCal category"
                  << "\n";
        return false;
    }

    // create Hit category
    catFibersHit = sifi()->buildCategory(SCategory::CatFibersStackHit);
    if (!catFibersHit)
    {
        std::cerr << "No CatFibersStackHit category"
                  << "\n";
        return false;
    }

    // get calibrator fiber parameters
    pHitFinderFiberPar =
    dynamic_cast<SCalContainer<2>*>(pm()->getCalibrationContainer("SFibersStackHitFinderFiberPar"));
    if (!pHitFinderFiberPar)
    {
        std::cerr << "Parameter container 'SFibersStackHitFinderFiberPar' was not obtained!" << std::endl; exit(EXIT_FAILURE);
    }

    SCalPar<2> * def = new SCalPar<2>();
    def->par[0] = 0.0;
    def->par[1] = 1.0;

    pHitFinderFiberPar->setDefault(def);

    return true;
}

/**
 * Execute task
 *
 * \sa STask::execute()
 * \return success
 */
bool SFibersStackHitFinder::execute()
{
    int size = catFibersCal->getEntries(); // changed from Raw
    for (int i = 0; i < size; ++i)
    {
        SFibersStackCal* pCal =
            dynamic_cast<SFibersStackCal*>(catFibersCal->getObject(i));
        if (!pCal)
        {
            printf("FibersStackCal doesn't exists!\n");
            continue;
        }
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pCal->getAddress(mod, lay, fib);

        // calc laboratory coordinates from digi data
        Float_t u = pCal->getU();
        Float_t y = pCal->getY();
        Float_t qdc_l = pCal->getQDCL();
        Float_t qdc_r = pCal->getQDCR();
        Float_t time_l = pCal->getTimeL();
        Float_t time_r = pCal->getTimeR();

        // if times are small, then sipm hit for given side was not recorded
        if (fabs(time_l) < 0.0001 or fabs(time_r) < 0.0001) continue;

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackHit* pHit =
            dynamic_cast<SFibersStackHit*>(catFibersHit->getObject(loc));
        if (!pHit)
        {
            pHit =
                reinterpret_cast<SFibersStackHit*>(catFibersHit->getSlot(loc));
            new (pHit) SFibersStackHit;
            pHit->Clear();
        }

        pHit->setAddress(mod, lay, fib);

        // obtain per fiber hit finder parameters
        SLookupChannel chan;
        chan.m = mod;
        chan.l = lay;
        chan.s = fib;

        SCalPar<2>* hfp = pHitFinderFiberPar->getPar(&chan);
        Float_t a0 = hfp->par[0];
        Float_t lambda = hfp->par[1];

        // calculate position from MLR
        Float_t hitPosM = (log(sqrt(qdc_r/qdc_l)) - a0) * lambda;
        pHit->setXYZ(hitPosM, 0, 0);

        // calculate position from times (inacccurate)
        Float_t v = 3e8 * 1e3 * 1e-9 / 1.82; // c * m/mm * ns/s / n_scint
        Float_t hitPosTime = ((time_l - time_r) * v) / 2;
        pHit->setXt(hitPosTime);
        pHit->setXtError(0.0);
    }

    return true;
}

/**
 * Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SFibersStackHitFinder::finalize() { return true; }
