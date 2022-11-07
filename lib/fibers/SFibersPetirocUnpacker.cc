// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersPetirocUnpacker.h"
#include "SCategory.h"
#include "SFibersCal.h"
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SLocator.h" // for SLocator
#include "SLookup.h"  // for SLookupChannel, SLookupTable
#include "SPetirocSource.h"
#include "SiFi.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersPetirocUnpacker::SFibersPetirocUnpacker()
    : SUnpacker(), catFibersRaw(nullptr), catFibersCal(nullptr), pLookUp(nullptr)
{
}

bool SFibersPetirocUnpacker::init()
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

    pLookUp = dynamic_cast<SFibersLookupTable*>(rdb()->getLookupContainer("FibersPETIROCLookupTable"));
    //     pLookUp->print();

    return true;
}

bool SFibersPetirocUnpacker::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{
    // getting buffer
    PetirocHit* hit = static_cast<PetirocHit*>(buffer);
    if (!hit) return false;

    SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, hit->fiberID) );
    if (!lc) {
        std::cerr << "No associated channel<->fiber value in " << pLookUp->GetName() << std::endl;
        std::cerr << "The container(params.txt) might be empty or the channel, fiber information doesn't exist." << std::endl;
        exit(0);
    }
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
