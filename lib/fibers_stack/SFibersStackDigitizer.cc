// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackDigitizer.h"
#include "SFibersStackDigitizerPar.h"
#include "SFibersStackGeomPar.h"
#include "SGeantFibersRaw.h"
#include "SFibersStackCalSim.h"
#include "SParManager.h"
#include "SCategory.h"
#include "SiFi.h"

#include <iostream>

/** \class SFibersStackDigitizer
\ingroup lib_fibers_stack

A digitizer task.

\sa STask
*/

/** Constructor
 */
SFibersStackDigitizer::SFibersStackDigitizer() : STask(), catGeantFibersRaw(nullptr), pDigiPar(nullptr), pGeomPar(nullptr)
{
}

/** Destructor
 */
SFibersStackDigitizer::~SFibersStackDigitizer()
{
}

/** Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersStackDigitizer::init()
{
    catGeantFibersRaw = sifi()->getCategory(SCategory::CatGeantFibersRaw);
    if (!catGeantFibersRaw)
    {
        std::cerr << "No CatGeantFibersRaw category" << "\n";
        return false;
    }

    catFibersCalSim = sifi()->buildCategory(SCategory::CatFibersStackCal);
    if (!catFibersCalSim)
    {
        std::cerr << "No CatFibersStackCal category" << "\n";
        return false;
    }

    pGeomPar = (SFibersStackGeomPar*) pm()->getParameterContainer("SFibersStackGeomPar");
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'SFibersStackGeomPar' was not obtained!" << std::endl;
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

/** Execute task
 * \sa STask::execute()
 * \return success
 */
bool SFibersStackDigitizer::execute()
{
    int size = catGeantFibersRaw->getEntries();

    for (int i = 0; i < size; ++i)
    {
        SGeantFibersRaw * pHit = (SGeantFibersRaw *)catGeantFibersRaw->getObject(i);
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
                    fib = address - layer_fiber_limit[mod][l-1];
                else
                    fib = address;
                break;
            }
        }

        Float_t u = pGeomPar->getFiberOffsetX(mod, lay) + fib * pGeomPar->getFibersPitch(mod, lay);
        Float_t y = pGeomPar->getFiberOffsetY(mod, lay);

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackCalSim * pCal = (SFibersStackCalSim *) catFibersCalSim->getObject(loc);
        if (!pCal)
        {
            pCal = (SFibersStackCalSim *) catFibersCalSim->getSlot(loc);
            pCal = new (pCal) SFibersStackCalSim;
        }

        pCal->setAddress(mod, lay, fib);
        pCal->setU(u);
        pCal->setY(y);
        pCal->setEnergyLoss(pHit->getEnergyLoss());
        pCal->setEnergyDeposition(pHit->getEnergyDeposition());
        pCal->setKineticEnergy(pHit->getKineticEnergy());
        pCal->setTotalEnergy(pHit->getTotalEnergy());
    }

    return true;
}

/** Finalize task
 * \sa STask::finalize()
 * \return success
 */
bool SFibersStackDigitizer::finalize()
{
    return true;
}
