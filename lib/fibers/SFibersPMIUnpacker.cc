// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersPMIUnpacker.h"
#include "SCategory.h"
#include "SDatabase.h" // for pm, SDatabase
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SLocator.h" // for SLocator
#include "SLookup.h"  // for SLookupChannel, SLookupTable
#include "SPMISource.h"
#include "SiFi.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersPMIUnpacker::SFibersPMIUnpacker()
    : SUnpacker(), catFibersRaw(nullptr), catFibersCal(nullptr), pLookUp(nullptr)
{
}

bool SFibersPMIUnpacker::init()
{
    SUnpacker::init();

    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersRaw);

    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersRaw category" << std::endl;
        return false;
    }

    catFibersCal = sifi()->buildCategory(SCategory::CatFibersCal);

    if (!catFibersCal)
    {
        std::cerr << "No CatFibersCal category" << std::endl;
        return false;
    }

    pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersPMILookupTable"));
    //     pLookUp->print();

    return true;
}

bool SFibersPMIUnpacker::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{
    // getting buffer
    PMIHit* hit = static_cast<PMIHit*>(buffer);

    if (!hit) return false;

    SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, hit->fiberID));
    SLocator loc(3);
    loc[0] = lc->m; // mod;
    loc[1] = lc->l; // lay;
    loc[2] = lc->s; // fib;

    // setting category
    SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(loc));
    if (!pRaw)
    {
        pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
        new (pRaw) SFibersRaw;
    }

    pRaw->setAddress(loc[0], loc[1], loc[2]);

    pRaw->setQDCL(hit->qdc_l);
    pRaw->setTimeL(hit->time_l);
    pRaw->setQDCR(hit->qdc_r);
    pRaw->setTimeR(hit->time_r);

    return true;
}
