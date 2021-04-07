// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersHitFinder.h"
#include "SCategory.h"
#include "SFibersCalSim.h"
#include "SFibersGeomPar.h"
#include "SFibersHitFinderPar.h"
#include "SFibersHitSim.h"
#include "SLocator.h"
#include "SLookup.h"
#include "SiFi.h"

#include <RtypesCore.h>

#include <cstdio>
#include <iostream>
#include <math.h>

/**
 * \class SFibersHitFinder
\ingroup lib_fibers

A hit finder task for Fibers Stack.

Takes each fiber data and tries to reconstruct hit along the fiber.

*/

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersHitFinder::init()
{

    // get Cal category
    catFibersCal = sifi()->getCategory(SCategory::CatFibersCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersCal category" << std::endl;
        return false;
    }

    // create Hit category
    catFibersHit = sifi()->buildCategory(SCategory::CatFibersHit);
    if (!catFibersHit)
    {
        std::cerr << "No CatFibersHit category" << std::endl;
        return false;
    }

    // get calibrator fiber parameters
    pHitFinderFiberPar =
        dynamic_cast<SCalContainer<3>*>(pm()->getCalContainer("FibersHitFinderFiberPar"));
    if (!pHitFinderFiberPar)
    {
        std::cerr << "Parameter container 'SFibersHitFinderFiberPar' was not obtained!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    auto def = new SCalPar<3>();
    (*def)[0] = 0.0;
    (*def)[1] = 1.0;
    (*def)[2] = 1.0;

    pHitFinderFiberPar->setDefault(def);

    // get calibrator parameters
    pHitFinderPar = dynamic_cast<SFibersHitFinderPar*>(pm()->getParContainer("FibersHitFinderPar"));
    if (!pHitFinderPar)
    {
        std::cerr << "Parameter container 'SFibersHitFinderPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    pGeomPar = dynamic_cast<SFibersGeomPar*>(pm()->getParContainer("FibersGeomPar"));
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'SFibersGeomPar' was not obtained!" << std::endl;
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
bool SFibersHitFinder::execute()
{
    int size = catFibersCal->getEntries(); // changed from Raw
    for (int i = 0; i < size; ++i)
    {
        SFibersCal* pCal = dynamic_cast<SFibersCal*>(catFibersCal->getObject(i));
        if (!pCal)
        {
            printf("FibersCal doesn't exists!\n");
            continue;
        }
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pCal->getAddress(mod, lay, fib);

        // calc laboratory coordinates from digi data
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

        SFibersHit* pHit = dynamic_cast<SFibersHit*>(catFibersHit->getObject(loc));
        if (!pHit)
        {
            pHit = reinterpret_cast<SFibersHit*>(catFibersHit->getSlot(loc));
            if (sifi()->isSimulation())
                pHit = new (pHit) SFibersHitSim;
            else
                pHit = new (pHit) SFibersHit;
            pHit->Clear();
        }

        pHit->setAddress(mod, lay, fib);

        // obtain per fiber hit finder parameters
        SLookupChannel chan;
        chan.m = mod;
        chan.l = lay;
        chan.s = fib;

        Float_t a0 = 0.0;
        Float_t lambda = 0.0;
        Float_t alpha = 0.0;

        if (sifi()->isSimulation())
        {
            a0 = pHitFinderPar->getA0();
            lambda = pHitFinderPar->getLambda();
            alpha = pHitFinderPar->getAlpha();
        }
        else
        {
            auto* hfp = pHitFinderFiberPar->getPar(&chan);
            a0 = (*hfp)[0];
            lambda = (*hfp)[1];
            alpha = (*hfp)[2];
        }

        Float_t rot = pGeomPar->getLayerRotation(loc[0], loc[1]);

        // calculate position from MLR
        Float_t u = (log(sqrt(qdc_r / qdc_l)) - a0) * lambda;
        Float_t s_u = 5.;

        // the fiber is taken from geometry
        Float_t v = pGeomPar->getFiberOffsetX(mod, lay) + fib * pGeomPar->getFibersPitch(mod, lay);
        Float_t s_v = 0.65;

        Float_t x = v * cos(rot * M_PI / 180) + u * sin(rot * M_PI / 180);
        Float_t y = v * sin(rot * M_PI / 180) + u * cos(rot * M_PI / 180);

        Float_t s_x = s_v * cos(rot * M_PI / 180) + s_u * sin(rot * M_PI / 180);
        Float_t s_y = s_v * sin(rot * M_PI / 180) + s_u * cos(rot * M_PI / 180);

        Float_t z = pGeomPar->getFiberOffsetY(mod, lay) + pGeomPar->getModuleZ(mod);
        Float_t s_z = 0.65;

        pHit->getPoint().SetXYZ(x, y, z);
        pHit->getErrors().SetXYZ(s_x, s_y, s_z);
        pHit->setU(u, 10.);

        // calculate energy
        Float_t E = alpha * sqrt(qdc_r * qdc_l);
        pHit->setE(E, 0);

        SFibersCalSim* pCalSim = dynamic_cast<SFibersCalSim*>(pCal);
        if (sifi()->isSimulation() and pCalSim)
        {
            ((SFibersHitSim*)pHit)->setGeantEnergyLoss(pCalSim->getGeantEnergyLoss());
            ((SFibersHitSim*)pHit)->getGeantPoint() = pCalSim->getGeantPoint();
        }
    }

    return true;
}

/**
 * Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SFibersHitFinder::finalize() { return true; }
