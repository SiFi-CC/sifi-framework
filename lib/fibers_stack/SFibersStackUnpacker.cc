// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackUnpacker.h"
#include "SFibersStackRaw.h"
#include "SParManager.h"
#include "SCategory.h"
#include "SiFi.h"

#include <iostream>

/**
 * \class SFibersStackUnpacker
\ingroup lib_fibers_stack

A unpacker task.

\sa STask
*/

/**
 * Constructor
 */
SFibersStackUnpacker::SFibersStackUnpacker() : STask(), catFibersRaw(nullptr)
{
}

/**
 * Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersStackUnpacker::init()
{
    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersStackRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersStackRaw category" << "\n";
        return false;
    }

    return true;
}

/**
 * Execute task
 * \sa STask::execute()
 * \return success
 */
bool SFibersStackUnpacker::execute()
{
    // get input here

    // determine data size
    int size = 0;

    for (int i = 0; i < size; ++i)
    {
        // do something with the data

        // calculate address
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;

        // take these from lookup table?
        Float_t u = 0;
        Float_t y = 0;

        // take this from data
        Float_t adc = 0;

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersStackRaw * pRaw = dynamic_cast<SFibersStackRaw *>(catFibersRaw->getObject(loc));
        if (!pRaw)
        {
            pRaw = dynamic_cast<SFibersStackRaw *>(catFibersRaw->getSlot(loc));
            new (pRaw) SFibersStackRaw;
            pRaw->Clear();
        }

        pRaw->setAddress(mod, lay, fib);
        pRaw->setU(u);
        pRaw->setY(y);
        pRaw->setADCL(adc);
        pRaw->setADCR(adc);
        pRaw->setTimeL(0);
        pRaw->setTimeR(0);
    }

    return true;
}

/**
 * Finalize task
 * \sa STask::finalize()
 * \return success
 */
bool SFibersStackUnpacker::finalize()
{
    return true;
}
