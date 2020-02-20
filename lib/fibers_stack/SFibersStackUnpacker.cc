// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include "SFibersStackUnpacker.h"
#include "SFibersStackRaw.h"

#include "SParManager.h"
#include "SCategory.h"

/** \class SFibersStackUnpacker
\ingroup lib_fibers_stack

A unpacker task.

\sa STask
*/

/** Constructor
 */
SFibersStackUnpacker::SFibersStackUnpacker() : STask(), catFibersRaw(nullptr)
{
}

/** Destructor
 */
SFibersStackUnpacker::~SFibersStackUnpacker()
{
}

/** Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersStackUnpacker::init()
{
    catFibersRaw = mapt()->buildCategory(SCategory::CatFibersStackRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersStackRaw category" << "\n";
        return false;
    }

    return true;
}

/** Execute task
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

        SFibersStackRaw * pRaw = (SFibersStackRaw *) catFibersRaw->getObject(loc);
        if (!pRaw)
        {
            pRaw = (SFibersStackRaw *) catFibersRaw->getSlot(loc);
            pRaw = new (pRaw) SFibersStackRaw;
        }

        pRaw->setAddress(mod, lay, fib);
        pRaw->setU(u);
        pRaw->setY(y);
        pRaw->setADC(adc);
    }

    return true;
}

/** Finalize task
 * \sa STask::finalize()
 * \return success
 */
bool SFibersStackUnpacker::finalize()
{
    return true;
}
