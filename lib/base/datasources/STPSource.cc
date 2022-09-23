// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "STPSource.h"
#include "SUnpacker.h"
#include "SLookup.h"
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
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
STPSource::STPSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(),  /*istream(), */ input_file(), t(), /*state(INIT),*/ hit_cache(nullptr)
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool STPSource::open()
{
    input_file = new TFile(input.c_str());
    if(!input_file->IsOpen()) {
        std::cerr << "##### Error in STPSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }
    entries_counter = 0;
    t = (TTree*)input_file->Get("data");
    nentries=t->GetEntries();
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

bool STPSource::close()
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

bool STPSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;
    
    std::vector<std::shared_ptr<TPHit>> hits;

    hit_cache = std::make_shared<TPHit>();
    t->SetBranchAddress("time",&(hit_cache->time));
    t->SetBranchAddress("channelID",&(hit_cache->channelID));
    t->SetBranchAddress("energy",&(hit_cache->energy));

    t->GetEntry(getCurrentEvent() );
    entries_counter++;

    if(entries_counter == nentries) return true;
    hits.push_back(hit_cache);
    int n_hits = hits.size();
    
    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setSampleTimeBin(1.);
        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < n_hits; i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, 0, subevent, hits[i].get(), 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
            u.second->setSampleTimeBin(1.);
            u.second->setADCTomV(1.);
            for (int i = 0; i < n_hits; i++)
            {
                u.second->execute(0, 0, u.first, hits[i].get(), 0);
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
void STPSource::setInput(const std::string& filename, size_t length) { input = filename; }

