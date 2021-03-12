// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersUnpacker.h"
#include "SCategory.h"
#include "SFibersRaw.h"
#include "SParManager.h"
#include "SiFi.h"

#include <iostream>

/**
 * \class SFibersUnpacker
\ingroup lib_fibers

A unpacker task.

\sa STask
*/

/**
 * Constructor
 */
SFibersUnpacker::SFibersUnpacker() : STask(), catFibersRaw(nullptr) {}

/**
 * Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersUnpacker::init()
{
    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersRaw);
    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersRaw category" << std::endl;
        return false;
    }

    return true;
}

/**
 * Execute task
 * \sa STask::execute()
 * \return success
 */
bool SFibersUnpacker::execute()
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

        // take this from data
        Float_t qdc = 0;

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(loc));
        if (!pRaw)
        {
            pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
            new (pRaw) SFibersRaw;
            pRaw->Clear();
        }

        pRaw->setAddress(mod, lay, fib);
        pRaw->setQDCL(qdc);
        pRaw->setQDCR(qdc);
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
bool SFibersUnpacker::finalize() { return true; }
