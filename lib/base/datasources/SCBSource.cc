// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SCBSource.h"
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
#include <SFibersLookup.h>
#include <SLocator.h>
/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SCBSource::SCBSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), istream(), state(INIT), hit_cache(nullptr), address(nullptr)
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SCBSource::open()
{
    istream.open(input.c_str());
    if (!istream.is_open())
    {
        std::cerr << "##### Error in SCBSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    std::string header;
    for(int i=0; i<9; i++)
    {
        getline(istream, header);
    }
    
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

bool SCBSource::close()
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

    istream.close();
    return true;
}


bool operator==(const SCBSource::Address& a, const SCBSource::Address& b) 
{ 
    return a.mod==b.mod && a.lay==b.lay && a.fib==b.fib;
} 

bool SCBSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;
    
    const double us_to_ns = 1E-3;
    std::string csvLine;
    std::string tmp;
    std::string str_tstamp_us, str_trgID, str_brd, str_ch, str_q_lg, str_q_hg;
    float tstamp_us = 0.;
    float q_lg = 0.;
    int trgID = 0;
    int brd = 0; 
    int ch = 0;
    std::string q_hg;
    int not_spaces=0;
    Address key;
    
    std::vector<Address> addresses;
    std::vector<std::shared_ptr<CBHit>> hits;
    std::vector<std::shared_ptr<Address>> shared_addresses;
    SFibersLookupTable* pLookUp;
    pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersPMILookupTable"));
    SLocator loc(3);
    
    if (state == DONE) { return false; }

    if (state == INIT)
    {
        hit_cache = std::make_shared<CBHit>();
        address = std::make_shared<Address>();
        getline(istream, csvLine);
        not_spaces=0;
        for(int j=0; j<csvLine.length(); j++){
            if(csvLine[j]!=' ') not_spaces++;
        }
        if(not_spaces <= 10){
            std::cerr << "not a valid event!" << std::endl;
        }
        
        if(not_spaces > 10){
            std::stringstream stream(csvLine);
            stream >> str_tstamp_us >> str_trgID >> str_brd >> str_ch >> str_q_lg >> str_q_hg;
            tstamp_us = std::stof(str_tstamp_us);
            trgID = std::stoi(str_trgID);
            brd = std::stoi(str_brd);
            ch = std::stoi(str_ch);
            q_lg = std::stof(str_q_lg);
            q_hg = str_q_hg;
            hit_cache->trgID = trgID;
            hit_cache->tstamp_us = tstamp_us;
            hit_cache->brd = brd;
            hit_cache->ch = ch;
            hit_cache->q_lg = q_lg;
            hit_cache->q_hg = q_hg;
            
            SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hit_cache->ch));
                loc[0] = lc->m; // mod;
                loc[1] = lc->l; // lay;
                loc[2] = lc->s; // fib;
                char side = lc->side;
                
                std::cout << "INIT m, l, f, side; ch " << loc[0] << " "<< loc[1] << " "<< loc[2] << " "<< side << " " << hit_cache->ch << std::endl;
                
                if(side=='l'){
                    hit_cache->time_l = tstamp_us / us_to_ns;
                    hit_cache->qdc_l = q_lg;
                }
                if(side=='r'){
                    hit_cache->time_r = tstamp_us / us_to_ns;
                    hit_cache->qdc_r = q_lg;
                }
                
                addresses.push_back({loc[0], loc[1], loc[2]});
                address->mod = loc[0];
                address->lay = loc[1];
                address->fib = loc[2];            
        }
        

        state = READING;
    }

    if (state == READING)
    {
        hits.push_back(hit_cache);
        shared_addresses.push_back(address);
        addresses.push_back({address->mod, address->lay, address->fib});
        
        while (true)
        {
            auto hit_current = std::make_shared<CBHit>();
            getline(istream, csvLine);
            not_spaces=0;
            for(int j=0; j<csvLine.length(); j++){
                if(csvLine[j]!=' ') not_spaces++;
            }


            if (!istream.good())
            {
                state = DONE;
                break;
            }

            if (not_spaces <= 10) // same event?
            {
                std::stringstream stream(csvLine);
                stream >> str_brd >> str_ch >> str_q_lg >> str_q_hg;
                brd = std::stoi(str_brd);
                ch = std::stoi(str_ch);
                q_lg = std::stof(str_q_lg);
                q_hg = str_q_hg;
                hit_current->brd = brd;
                hit_current->ch = ch;
                hit_current->q_lg = q_lg;
                hit_current->q_hg = q_hg;
                hit_current->tstamp_us = hit_cache->tstamp_us;
                hit_current->trgID = hit_cache->trgID;
                
                SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hit_current->ch));

                loc[0] = lc->m; // mod;
                loc[1] = lc->l; // lay;
                loc[2] = lc->s; // fib;
                char side = lc->side;
                key = {loc[0], loc[1], loc[2]};
                if(std::find(addresses.begin(), addresses.end(), key) != addresses.end())
                {
                    int index = std::find(addresses.begin(), addresses.end(), key) - addresses.begin();
                    if(side=='l'){
                        hits[index]->time_l = hit_cache->tstamp_us / us_to_ns;
                        hits[index]->qdc_l = q_lg;
                    }
                    if(side=='r'){
                        hits[index]->time_r = hit_cache->tstamp_us / us_to_ns;
                        hits[index]->qdc_r = q_lg;
                    }
                }
                
                else
                {
                    if(side=='l'){
                        hit_current->time_l = hit_cache->tstamp_us / us_to_ns;
                        hit_current->qdc_l = q_lg;
                    }
                    if(side=='r'){
                        hit_current->time_r = hit_cache->tstamp_us / us_to_ns;
                        hit_current->qdc_r = q_lg;
                    }
                    
                    addresses.push_back({loc[0], loc[1], loc[2]});

                    address->mod = loc[0];
                    address->lay = loc[1];
                    address->fib = loc[2]; 
                    
                    hits.push_back(hit_current);
                    shared_addresses.push_back(address);
                }
            }
            else if(not_spaces > 10) // next event
            {
                addresses.clear();
                std::stringstream stream(csvLine);
                stream >> str_tstamp_us >> str_trgID >> str_brd >> str_ch >> str_q_lg >> str_q_hg;
                tstamp_us = std::stof(str_tstamp_us);
                trgID = std::stoi(str_trgID);
                brd = std::stoi(str_brd);
                ch = std::stoi(str_ch);
                q_lg = std::stof(str_q_lg);
                q_hg = str_q_hg;
                hit_current->trgID = trgID;
                hit_current->tstamp_us = tstamp_us;
                hit_current->brd = brd;
                hit_current->ch = ch;
                hit_current->q_lg = q_lg;
                hit_current->q_hg = q_hg;
                SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hit_current->ch));
                loc[0] = lc->m; // mod;
                loc[1] = lc->l; // lay;
                loc[2] = lc->s; // fib;
                char side = lc->side;
                if(side=='l'){
                    hit_current->time_l = tstamp_us / us_to_ns; //conversion to ns
                    hit_current->qdc_l = q_lg;
                }
                if(side=='r'){
                    hit_current->time_r = tstamp_us / us_to_ns; //conversion to ns
                    hit_current->qdc_r = q_lg;
                }
                
                addresses.push_back({loc[0], loc[1], loc[2]});
                
                address->mod = loc[0];
                address->lay = loc[1];
                address->fib = loc[2]; 
                hit_cache = hit_current;
                break;
            }
            else std::cerr << "Number of characters in the line of the input file undefined" <<std::endl;
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
void SCBSource::setInput(const std::string& filename, size_t length) { input = filename; }

