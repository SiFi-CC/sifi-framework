// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SSiFiCCDetResImporter.h"
#include "SCategory.h"
#include "SFibersStackCal.h"
#include "SFibersStackLookup.h"
#include "SParManager.h"
#include "SiFi.h"

#include <algorithm>
#include <iostream>
#include <numeric>

const Float_t adc_to_mv = 4.096;
const Float_t sample_to_ns = 1.;
/**
 * \class SSiFiCCDetResImporter
\ingroup lib_fibers_stack

A unpacker task.

\sa STask
*/

/**
 * Constructor
 */
SSiFiCCDetResImporter::SSiFiCCDetResImporter()
    : SUnpacker(), catFibersCal(nullptr), pLookUp(nullptr)
{
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SSiFiCCDetResImporter::init()
{
    SUnpacker::init();

    catFibersCal = sifi()->buildCategory(SCategory::CatFibersStackCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersStackCal category"
                  << "\n";
        return false;
    }

    pLookUp = dynamic_cast<SFibersStackLookupTable*>(
        pm()->getLookupContainer("SFibersStackDDLookupTable"));
    pLookUp->print();

    return true;
}

bool SSiFiCCDetResImporter::execute(ulong /*event*/, ulong /*seq_number*/,
                                    uint16_t /*subevent*/, void* buffer,
                                    size_t /*length*/)
{
    TREE_all* tree = static_cast<TREE_all*>(buffer);
    if (!tree) return false;

    SLocator loc(3);
    loc[0] = tree->address.m; // mod;
    loc[1] = tree->address.l; // lay;
    loc[2] = tree->address.f; // fib;
    char side = tree->address.s;

    SFibersStackCal* pCal =
        dynamic_cast<SFibersStackCal*>(catFibersCal->getObject(loc));
    if (!pCal)
    {
        pCal = reinterpret_cast<SFibersStackCal*>(catFibersCal->getSlot(loc));
        new (pCal) SFibersStackCal;
        pCal->setAddress(loc[0], loc[1], loc[2]);
    }

    // fill CALL
    if (side == 'l' and pCal->getQDCL() == 0.0)
    {
        pCal->setQDCL(tree->data.counts);
        pCal->setTimeL(tree->data.time);
    }
    if (side == 'r' and pCal->getQDCR() == 0.0)
    {
        pCal->setQDCR(tree->data.counts);
        pCal->setTimeR(tree->data.time);
    }

    return true;
}
