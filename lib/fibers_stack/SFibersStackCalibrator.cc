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
#include "SCalContainer.h"
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
    pCalibratorPar = dynamic_cast<SCalContainer*>(pm()->getCalibrationContainer("SFibersStackCalibratorPar"));
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

    for (int i = 0; i < size; ++i)
    {
        SFibersStackRaw * pRaw = dynamic_cast<SFibersStackRaw *>(catFibersRaw->getObject(i));
        if (!pRaw)
        {
            printf("Hit doesnt exists!\n");
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

        SCalPar & cp_l = pCalibratorPar->getPar(&chan_l);
        SCalPar & cp_r = pCalibratorPar->getPar(&chan_r);

        // calc laboratory coordinates from digi data
        Float_t u = pRaw->getU();
        Float_t y = pRaw->getY();
        Float_t adc_l = pRaw->getADCL();
        Float_t adc_r = pRaw->getADCR();
        Float_t time_l = pRaw->getTimeL();
        Float_t time_r = pRaw->getTimeR();

        // do your magic here with u, y and adc
        Float_t lab_u = u;
        Float_t lab_y = y;
        Float_t energy_l = cp_l.par0 * adc_l + cp_l.par1;
        Float_t energy_r = cp_r.par0 * adc_r + cp_r.par1;
        time_l += cp_l.par2;
        time_r += cp_r.par2;

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackCal * pCal = dynamic_cast<SFibersStackCal *>(catFibersCal->getObject(loc));
        if (!pCal)
        {
            pCal = reinterpret_cast<SFibersStackCal *>(catFibersCal->getSlot(loc));
            new (pCal) SFibersStackCal;
            pCal->Clear();
        }

        pCal->setAddress(mod, lay, fib);
        pCal->setU(lab_u);
        pCal->setY(lab_y);
        pCal->setADC(energy_l, energy_r);
        pCal->setTime(time_l, time_r);
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
