// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersTP4to1Unpacker.h"
#include "SCategory.h"
#include "SDatabase.h" // for pm, SDatabase
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SLocator.h" // for SLocator
#include "SLookup.h"  // for SLookupChannel, SLookupTable
//#include "SPMISource.h"
#include "STPSource.h"
#include "SFibersIdentification.h" //for identifiedFiberData structure. careful! is it a cross reference?
#include "SiFi.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersTP4to1Unpacker::SFibersTP4to1Unpacker()
    : SUnpacker(), catFibersRaw(nullptr), catFibersCal(nullptr)
{
}

bool SFibersTP4to1Unpacker::init()
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
    

    return true;
}

bool SFibersTP4to1Unpacker::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{
    // getting buffer
    identifiedFiberData* fiber_hit = static_cast<identifiedFiberData*>(buffer);

    if (!fiber_hit) return false;

    SLocator loc(3);
    loc[0] = fiber_hit->mod; // mod;
    loc[1] = fiber_hit->lay; // lay;
    loc[2] = fiber_hit->fi; // fib;
    
    // setting category
    SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(loc));
    if (!pRaw)
    {
        pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
        new (pRaw) SFibersRaw;
    }

    pRaw->setAddress(loc[0], loc[1], loc[2]);
    pRaw->setQDCL(fiber_hit->energyL);
    pRaw->setTimeL(fiber_hit->timeL);
    pRaw->setQDCR(fiber_hit->energyR);
    pRaw->setTimeR(fiber_hit->timeR);
        
    return true;
}
