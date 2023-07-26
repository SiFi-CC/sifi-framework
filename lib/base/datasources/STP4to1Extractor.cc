#include "STP4to1Source.h"
#include "STP4to1Extractor.h"
#include "SFibersIdentification.h"
#include "SUnpacker.h"
#include "SFibersLookup.h"
#include "SLookup.h"
#include "SSiPMHit.h"
#include "SCategory.h"
#include "SiFi.h"
#include "SLocator.h"
#include <algorithm>          // for max, min
#include <cmath>              // for fabs
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for abort
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <map>
#include <utility> // for pair
#include <vector>  // for vector
#include <sstream>

#include"TFile.h"
#include"TTree.h"

/**
 * Constructor. Requires subevent id for unpacked source and map of unpackers.
 *
 * \param subevent subevent id
 */
STP4to1Extractor::STP4to1Extractor(uint16_t subevent, std::map<uint16_t, SUnpacker*> unpackers)
    : subevent(subevent), unpackers(unpackers)
{
}

STP4to1Extractor::~STP4to1Extractor()
{
    //delete identification;
}


bool STP4to1Extractor::extract_open()
{
    if (unpackers.size() == 0) return false;
    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();

        bool res = unpackers[subevent]->init();
        if (!res)
        {
            printf("Forced unpacker %#x not initalized\n", subevent);
            abort();
        }
    }
    else
    {
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
        {
            bool res = iter->second->init();
            if (!res)
            {
                printf("Unpacker %#x not initalized\n", iter->first);
                abort();
            }
        }
    }
    return true;
}

bool STP4to1Extractor::extract_close()
{
    if (subevent != 0x0000)
    {
        if (unpackers[subevent])
            unpackers[subevent]->finalize();
        else
            abort();
    }
    else
    {
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->finalize();
    }

    return true;
}

bool STP4to1Extractor::extract_readCurrentEvent(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    
    SFibersIdentification identification;
    identification.init();

    std::vector fiberHits = identification.identifyFibers(hits);
    int n_fiber_hits = fiberHits.size();
    
    if (unpackers.size() == 0) return false;
    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setSampleTimeBin(1.);
        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < n_fiber_hits; i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, 0, subevent, fiberHits[i].get(), 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
            u.second->setSampleTimeBin(1.);
            u.second->setADCTomV(1.);
            for (int i = 0; i < n_fiber_hits; i++)
            {
                u.second->execute(0, 0, u.first, fiberHits[i].get(), 0);
            }
        }
    }

return true;     
}

bool STP4to1Extractor::write_to_tree(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    // get SiPMHit category
    SCategory * catSiPMHit = sifi()->buildCategory(SCategory::CatSiPMHit);
    if (!catSiPMHit)
    {
        std::cerr << "No CatSiPMHit category" << std::endl;
        return false;
    }

    SSiPMsLookupTable* pLookUp;
    pLookUp = dynamic_cast<SSiPMsLookupTable*>(pm()->getLookupContainer("TPLookupTable"));
    int n_hits = hits.size();
    for (int i = 0; i < n_hits; i++)
    {
        SLocator loc(1);
//        loc[0] = hits[i]->channelID; // changed for SiPM clustering!
        loc[0] = i; // new locator is hit ID
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
        SSiPMsChannel* lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[i]->channelID));
        if(!lc) {
            fprintf(stderr, "STP4to1Extractor TOFPET2 absolute Ch%d missing. Check params.txt.\n", hits[i]->channelID);
        } else {
            pHit->setChannel(lc->s);
            pHit->setAddress(lc->m, lc->l, lc->element, lc->side);
            pHit->setQDC(hits[i]->energy);
            pHit->setTime(hits[i]->time);
            pHit->setID(i);
        }
    }
 
return 0;    
}
