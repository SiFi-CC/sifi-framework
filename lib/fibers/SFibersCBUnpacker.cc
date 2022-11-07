// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersCBUnpacker.h"
#include "SCategory.h"
#include "SDatabase.h" // for pm, SDatabase
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SLocator.h" // for SLocator
#include "SLookup.h"  // for SLookupChannel, SLookupTable
#include "SPMISource.h"
#include "SCBSource.h"
#include "SiFi.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersCBUnpacker::SFibersCBUnpacker()
    : SUnpacker(), catFibersRaw(nullptr), catFibersCal(nullptr), pLookUp(nullptr)
{
}

bool SFibersCBUnpacker::init()
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

    pLookUp = dynamic_cast<SFibersLookupTable*>(rdb()->getLookupContainer("FibersPMILookupTable"));
    //     pLookUp->print();

    return true;
}

bool SFibersCBUnpacker::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{
    // getting buffer
    CBHit* hit = static_cast<CBHit*>(buffer);

    if (!hit) return false;

//     std::cout << "hit->fiberID: " << hit->fiberID << std::endl;
//     std::cout << "hit->ch: " << hit->ch << std::endl;
//     SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, hit->fiberID));
    SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, hit->ch));
    if (!lc) {
        std::cerr << "No associated channel<->fiber value in " << pLookUp->GetName() << std::endl;
        std::cerr << "The container(params.txt) might be empty or the channel, fiber information doesn't exist." << std::endl;
        exit(0);
    }
    SLocator loc(3);
    loc[0] = lc->m; // mod;
    loc[1] = lc->l; // lay;
    loc[2] = lc->s; // fib;
    char side = lc->side;

//     std::cout << "side; ch " << side << " " << hit->ch << std::endl;

    // setting category
    SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(loc));
    if (!pRaw)
    {
        pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
        new (pRaw) SFibersRaw;
    }

    pRaw->setAddress(loc[0], loc[1], loc[2]);

    if(side == 'l'){
        pRaw->setQDCL(hit->q_lg);
        pRaw->setTimeL(hit->tstamp_us);
        pRaw->setQDCR(-100);
        pRaw->setTimeR(-100);
    }

    else if(side == 'r'){
        pRaw->setQDCL(-100);
        pRaw->setTimeL(-100);
        pRaw->setQDCR(hit->q_lg);
        pRaw->setTimeR(hit->tstamp_us);
    }

    else{
        std::cerr << "fiber side undefined!" << std::endl;
    }


    return true;
}
