// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "STP4to1Source.h"
// #include "STP4to1Extractor.h"
#include "SUnpacker.h"
#include "SLookup.h"
#include "TFile.h"
#include "TTree.h"
#include <algorithm>          // for max, min
#include <cmath>              // for fabs
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for abort
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <iomanip>
#include <map>
#include <utility> // for pair
#include <vector>  // for vector
#include <sstream>

#include "SFibersLookup.h"
#include "SLocator.h"
#include "SCategory.h"
#include "SiFi.h"
/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
STP4to1Source::STP4to1Source(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), input_file(), t(),  hit_cache(nullptr), state(INIT)
{
}

STP4to1Source::~STP4to1Source()
{
//     delete extractor;
}

/**
 * Open source and call Extractor.
 *
 * \return success
 */
bool STP4to1Source::open()
{
    input_file = new TFile(input.c_str());
    if(!input_file->IsOpen()) {
        std::cerr << "##### Error in STP4to1Source::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }
    entries_counter = 0;
    t = (TTree*)input_file->Get("events");
    nentries=t->GetEntries();
    std::cout << "nentries " << nentries << std::endl;
/*
    catSiPMHit = sifi()->buildCategory(SCategory::CatSiPMHit); // BUG po co?
    if (!catSiPMHit)
    {
        std::cerr << "Error in STP4to1Extractor.cc: No CatSiPMHit category" << std::endl;
        abort();
    }*/
//     extractor = new STP4to1Extractor(subevent, unpackers);
//     extractor->extract_open();

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

bool STP4to1Source::close()
{
//     extractor->extract_close();
    
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

bool STP4to1Source::readCurrentEvent()
{
    const double deltaT = 10; // 10 ns window
    const double ps_to_ns = 1E3;
    std::vector<std::shared_ptr<TP4to1Hit>> hits;
    
    if (state == DONE) { return false; }

    if (state == INIT)
    {
        if((entries_offset + entries_counter) >= nentries) state = DONE;
        hit_cache = std::make_shared<TP4to1Hit>();

        t->SetBranchAddress("time",&(hit_cache->time));
        t->SetBranchAddress("channelID",&(hit_cache->channelID));
        t->SetBranchAddress("energy",&(hit_cache->energy));
        t->GetEntry(entries_offset + entries_counter);
//         entries_counter++;
        hit_cache->time /= ps_to_ns;
        hit_cache->channelID -= 131072*2; //offset related to port on TOFPET PCI card

        state = READING;
    }

    if (state == READING)
    {
        double cache_time = hit_cache->time;

        while (true)
        {
            if((entries_offset + entries_counter) >= nentries) 
            {
                state = DONE;
                break;
            }
            auto hit_current = std::make_shared<TP4to1Hit>();
            
            t->SetBranchAddress("time",&(hit_current->time));
            t->SetBranchAddress("channelID",&(hit_current->channelID));
            t->SetBranchAddress("energy",&(hit_current->energy));
            t->GetEntry(entries_offset + entries_counter);
            hit_current->time /= ps_to_ns;
            hit_current->channelID -= 131072*2; //offset related to port on TOFPET PCI card
            double current_time = hit_current->time;

            if (fabs(cache_time - current_time) < deltaT) // same event?
            {           
                hits.push_back(hit_current);
                entries_counter++;
            }
            else // next event
            {
                hit_cache = hit_current;
                break;
            }
        }
    }

//     for(auto i : hits)
//         i->print();
    
//     extractor->extract_readCurrentEvent(hits);
//     extractor->write_to_tree(hits);

    int nhits = hits.size();

    if (unpackers.size() == 0) return false;
    
    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setSampleTimeBin(1.);
        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < nhits; i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, i, subevent, hits[i].get(), 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
            u.second->setSampleTimeBin(1.);
            u.second->setADCTomV(1.);
            for (int i = 0; i < nhits; i++)
            {
                u.second->execute(0, i, u.first, hits[i].get(), 0);
            }
        }
    }


    return true;
}

/**
 * Set input for the source.
 *
 * \param filename input file name
 * \param length length of buffer to read
 */
void STP4to1Source::setInput(const std::string& filename, size_t length) { input = filename; }

