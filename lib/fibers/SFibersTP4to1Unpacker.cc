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
#include "SLocator.h" // for SLocator
#include "STP4to1Source.h"
#include "SSiPMHit.h"
#include "SiFi.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersTP4to1Unpacker::SFibersTP4to1Unpacker()
    : SUnpacker(), catSiPMHit(nullptr), pLookUp(nullptr)
{
}

bool SFibersTP4to1Unpacker::init()
{
    SUnpacker::init();

    catSiPMHit = sifi()->buildCategory(SCategory::CatSiPMHit);

    if (!catSiPMHit)
    {
        std::cerr << "No CatSiPMHit category" << std::endl;
        return false;
    }
    
    pLookUp = dynamic_cast<SSiPMsLookupTable*>(pm()->getLookupContainer("TPLookupTable"));
    
    return true;
}

bool SFibersTP4to1Unpacker::execute(ulong /*event*/, ulong seq_number, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{

    
    TP4to1Hit *hit = static_cast<TP4to1Hit*>(buffer);

    SLocator loc(1);
//     loc[0] = hits[i]->channelID; // changed for SiPM clustering!
//     loc[0] = i;  // new locator is hit ID
    loc[0] = seq_number; // new locator is hit ID
    SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMHit->getObject(loc));
    if (!pHit)
    {
        pHit = reinterpret_cast<SSiPMHit*>(catSiPMHit->getSlot(loc));
        new (pHit) SSiPMHit;
        if (!pHit)
        {
            std::cerr << "Error in STP4to1Extractor.cc: no pHit category!" << std::endl;
        }
    }
    SSiPMsChannel* lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hit->channelID));
    if(!lc)
    {
        fprintf(stderr, "STP4to1Extractor TOFPET2 absolute Ch%d missing. Check params.txt.\n", hit->channelID);
    } 
    else
    {
        pHit->setChannel(lc->s);
        pHit->setAddress(lc->m, lc->l, lc->element, lc->side);
        pHit->setQDC(hit->energy);
        pHit->setTime(hit->time);
//         pHit->setID(i);
        pHit->setID(seq_number);
    }

    return true;
}
