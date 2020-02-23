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
#include "SFibersStackCalibratorPar.h"
#include "SFibersStackRaw.h"
#include "SFibersStackCal.h"
#include "SParManager.h"
#include "SCategory.h"
#include "SiFi.h"

#include <iostream>

/** \class SFibersStackCalibrator
\ingroup lib_fibers_stack

A calibraror task for Fibers Stack.

Takes MGeantFibersStackRaw data and applies calibration. See MTask for the
interface description.

*/

/** Default constructor
 */
SFibersStackCalibrator::SFibersStackCalibrator() : STask(), catFibersRaw(nullptr), catFibersCal(nullptr), pCalibratorPar(nullptr)
{
}

/** Default destructor
 */
SFibersStackCalibrator::~SFibersStackCalibrator()
{
}

/** Init task
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
        std::cerr << "No CatFibersStackRaw category" << "\n";
        return false;
    }

    // create Cal category
    catFibersCal = sifi()->buildCategory(SCategory::CatFibersStackCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersStackCal category" << "\n";
        return false;
    }

    // get calibrator parameters
    pCalibratorPar = (SFibersStackCalibratorPar*) pm()->getParameterContainer("SFibersStackCalibratorPar");
    if (!pCalibratorPar)
    {
        std::cerr << "Parameter container 'SFibersStackCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

/** Execute task
 *
 * \sa STask::execute()
 * \return success
 */
bool SFibersStackCalibrator::execute()
{
    int size = catFibersRaw->getEntries();

    Float_t adc_gain = pCalibratorPar->getAdcGain();
    Float_t adc_offset = pCalibratorPar->getAdcOffset();

    for (int i = 0; i < size; ++i)
    {
        SFibersStackRaw * pRaw = (SFibersStackRaw *)catFibersRaw->getObject(i);
        if (!pRaw)
        {
            printf("Hit doesnt exists!\n");
            continue;
        }

        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pRaw->getAddress(mod, lay, fib);

        // calc laboratory coordinates from digi data
        Float_t u = pRaw->getU();
        Float_t y = pRaw->getY();
        Float_t adc = pRaw->getADC();

        // do your magic here with u, y and adc
        Float_t lab_u = u;
        Float_t lab_y = y;
        Float_t energy = adc_gain * adc + adc_offset;

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackCal * pCal = (SFibersStackCal *) catFibersCal->getObject(loc);
        if (!pCal)
        {
            pCal = (SFibersStackCal *) catFibersCal->getSlot(loc);
            pCal = new (pCal) SFibersStackCal;
        }

        pCal->setAddress(mod, lay, fib);
        pCal->setU(lab_u);
        pCal->setY(lab_y);
        pCal->setEnergyDeposition(energy);
    }

    return true;
}

/** Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SFibersStackCalibrator::finalize()
{
    return true;
}
