// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

/**
 * \class SFibersDigitizer
\ingroup lib_fibers

A digitizer task.

\sa STask
*/

#include "SFibersDigitizer.h"
#include "SCategory.h"
#include "SDatabase.h"
#include "SFibersCalSim.h"
#include "SFibersGeomPar.h"
#include "SGeantFibersRaw.h"
#include "SLocator.h" // for SLocator
#include "SPar.h"     // for SPar
#include "SiFi.h"

#include <TObject.h> // for TObject

#include <algorithm>          // for max
#include <cstdio>             // for printf
#include <cstdlib>            // for exit, EXIT_FAILURE
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <memory> // for allocator_traits<>::value_type

/**
 * Constructor
 */
SFibersDigitizer::SFibersDigitizer()
    : STask(), catGeantFibersRaw(nullptr), catFibersCalSim(nullptr), pDigiPar(nullptr),
      pGeomPar(nullptr)
{
}

/**
 * Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersDigitizer::init()
{
    catGeantFibersRaw = sifi()->getCategory(SCategory::CatGeantFibersRaw);
    if (!catGeantFibersRaw)
    {
        std::cerr << "No CatGeantFibersRaw category" << std::endl;
        return false;
    }

    catFibersCalSim = sifi()->buildCategory(SCategory::CatFibersCal);
    if (!catFibersCalSim)
    {
        std::cerr << "No CatFibersCal category" << std::endl;
        return false;
    }

    pGeomPar = dynamic_cast<SFibersGeomPar*>(pm()->getParContainer("FibersGeomPar"));
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'SFibersGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    Int_t modules = pGeomPar->getModules();
    layer_fiber_limit.resize(modules);
    for (int m = 0; m < modules; ++m)
    {
        Int_t cnt_fibers = 0;
        Int_t layers = pGeomPar->getLayers(m);
        layer_fiber_limit[m].resize(layers);
        for (int l = 0; l < layers; ++l)
        {
            cnt_fibers += pGeomPar->getFibers(m, l);
            layer_fiber_limit[m][l] = cnt_fibers;
        }
    }
    return true;
}

/**
 * Execute task
 * \sa STask::execute()
 * \return success
 */
bool SFibersDigitizer::execute()
{
    int size = catGeantFibersRaw->getEntries();
    for (int i = 0; i < size; ++i)
    {
        SGeantFibersRaw* pHit = dynamic_cast<SGeantFibersRaw*>(catGeantFibersRaw->getObject(i));
        if (!pHit)
        {
            printf("Hit doesnt exists!\n");
            continue;
        }

        Int_t mod = 0;
        Int_t address = 0;

        Int_t lay = 0;
        Int_t fib = 0;

        pHit->getAddress(mod, address);
        int layers = pGeomPar->getLayers(mod);
        for (int l = 0; l < layers; ++l)
        {
            if (address < layer_fiber_limit[mod][l])
            {
                lay = l;
                if (l > 0)
                    fib = address - layer_fiber_limit[mod][l - 1];
                else
                    fib = address;
                break;
            }
        }

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersCalSim* pCal = dynamic_cast<SFibersCalSim*>(catFibersCalSim->getObject(loc));
        if (!pCal)
        {
            pCal = dynamic_cast<SFibersCalSim*>(catFibersCalSim->getSlot(loc));
            new (pCal) SFibersCalSim;
            pCal->Clear();
        }

        pCal->setAddress(mod, lay, fib);
        pCal->setQDCL(pHit->getLightL());
        pCal->setQDCR(pHit->getLightR());
        pCal->setGeantEnergyLoss(pHit->getEnergyLoss());
        pCal->setGeantPoint({0., 0., 0.}); // FIXME fetch data from geant
    }

    return true;
}

/**
 * Finalize task
 * \sa STask::finalize()
 * \return success
 */
bool SFibersDigitizer::finalize() { return true; }
