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
#include <SFibersLookup.h>
#include <SLocator.h>


/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
STPSource::STPSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), input_file(), t(),  hit_cache(nullptr), state(INIT), address(nullptr)
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
    try{
        std::cout << "trying to read tree \"events\"" << std::endl;
        t = (TTree*)input_file->Get("events");
        if(t) std::cout << "tree name: events" << std::endl;
        else throw 505;
    }
    catch(int errorNo) {
        std::cout << "tree name: not \"events\". trying to read in tree named \"data\" " << std::endl;
        t = (TTree*)input_file->Get("data");
    }
    nentries=t->GetEntries();
    std::cout << "nentries " << nentries << std::endl;
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

bool operator==(const STPSource::Address& a, const STPSource::Address& b) 
{ 
    return a.mod==b.mod && a.lay==b.lay && a.fib==b.fib;
} 

bool STPSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;

    const double deltaT = 10; // 10 ns window
    const double ps_to_ns = 1E3;
    Address key;
    std::vector<std::shared_ptr<TPHit>> hits;
    std::vector<Address> addresses;
    
    SFibersLookupTable* pLookUp;
    pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersPMILookupTable")); //TODO CHANGE THE CONTAINER NAME AFTER MERGE TO FibersTPLookupTable (ALSO IN OTHER PLACES)
    SLocator loc(3);

    if (state == DONE) { return false; }

    if (state == INIT)
    {
        hit_cache = std::make_shared<TPHit>();
        address = std::make_shared<Address>();

        t->SetBranchAddress("time",&(hit_cache->time));
        t->SetBranchAddress("channelID",&(hit_cache->channelID));
        t->SetBranchAddress("energy",&(hit_cache->energy));
        t->GetEntry(entries_counter);
        hit_cache->time /= ps_to_ns;
        entries_counter++;
        if(entries_counter == nentries) state = DONE;
        SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hit_cache->channelID));
        loc[0] = lc->m; // mod;
        loc[1] = lc->l; // lay;
        loc[2] = lc->s; // fib;
        char side = lc->side;
       
        if(side=='l'){
            hit_cache->time_l = hit_cache->time;
            hit_cache->qdc_l = hit_cache->energy;
        }
        if(side=='r'){
            hit_cache->time_r = hit_cache->time;
            hit_cache->qdc_r = hit_cache->energy;
        }

        addresses.push_back({loc[0], loc[1], loc[2]});
        hits.push_back(hit_cache);
        address->mod = loc[0];
        address->lay = loc[1];
        address->fib = loc[2]; 

        state = READING;
    }

    if (state == READING)
    {
        double cache_time = hit_cache->time;

        while (true)
        {
            auto hit_current = std::make_shared<TPHit>();
            
            t->SetBranchAddress("time",&(hit_current->time));
            t->SetBranchAddress("channelID",&(hit_current->channelID));
            t->SetBranchAddress("energy",&(hit_current->energy));
            
            
            if(entries_counter == nentries) 
            {
                std::cout << "FINcounter:   " << entries_counter << std::endl;
                state = DONE;
                break;
            }
            
            t->GetEntry(entries_counter);
            hit_current->time /= ps_to_ns;
            SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hit_current->channelID));
            loc[0] = lc->m; // mod;
            loc[1] = lc->l; // lay;
            loc[2] = lc->s; // fib;
            char side = lc->side;
            
            key = {loc[0], loc[1], loc[2]};
            double current_time = hit_current->time;

            if (fabs(cache_time - current_time) < deltaT) // same event?
            {
                if(std::find(addresses.begin(), addresses.end(), key) != addresses.end())
                {
                    
                    int index = std::find(addresses.begin(), addresses.end(), key) - addresses.begin();
                    if(side=='l'){
                        hits[index]->time_l = hit_current->time;
                        hits[index]->qdc_l = hit_current->energy;
                    }
                    if(side=='r'){
                        hits[index]->time_r = hit_current->time;
                        hits[index]->qdc_r = hit_current->energy;
                    }   
                }

                else
                {
                    if(side=='l'){
                        hit_current->time_l = hit_current->time;
                        hit_current->qdc_l = hit_current->energy;
                    }
                    if(side=='r'){
                        hit_current->time_r = hit_current->time;
                        hit_current->qdc_r = hit_current->energy;
                    }
                    addresses.push_back({loc[0], loc[1], loc[2]});
                    address->mod = loc[0];
                    address->lay = loc[1];
                    address->fib = loc[2]; 
                    
                    hits.push_back(hit_current);
                }
                entries_counter++;
            }
            else // next event
            {
                addresses.clear();
                hit_cache = hit_current;
                break;
            }
        }
    }

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

