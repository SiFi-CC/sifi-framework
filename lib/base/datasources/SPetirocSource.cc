// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SPetirocSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <sstream>
#include <map>

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SPetirocSource::SPetirocSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), istream()
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SPetirocSource::open()
{
    istream.open(input.c_str());
    if (!istream.is_open())
    {
        std::cerr << "##### Error in SPetirocSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    std::string header;
    getline(istream, header);

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

bool SPetirocSource::close()
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

bool SPetirocSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;

    std::vector<PetirocHit> hits;

    // Make sure the file is open
    if(!istream.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    std::string line, col;
    int val;

    // Read the column names
    std::vector<uint16_t> data;
    if(!istream.good() ) return false;
    // Extract the first line in the file
    std::getline(istream, line);
    // Create a stringstream from line
    std::stringstream ss(line);
    // Extract each column name
    while(std::getline(ss, col, ' ')){
        if(col == '\r') continue;
        data.push_back(std::stoi(col) );
    }
    if(data.size() == 0) return false;
    for(uint16_t i=0; i < 1; ++i) {
        PetirocHit hit_cache;
        hit_cache.time_l = (data[64 + i*2] + 1)*25 - data[i*2]*.037; //ns
        hit_cache.time_r = 0;
        hit_cache.qdc_l = data[i*2+1]; //adc
        hit_cache.qdc_r = 0;
        hit_cache.fiberID = i;
        hits.push_back(hit_cache);
    }
    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
//        unpackers[subevent]->setSampleTimeBin(1.);
//        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < hits.size(); i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, 0, subevent, &hits[i], 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
//            u.second->setSampleTimeBin(1.);
//            u.second->setADCTomV(1.);
            for (int i = 0; i < hits.size(); i++)
            {
                u.second->execute(0, 0, u.first, &hits[i], 0);
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
void SPetirocSource::setInput(const std::string& filename, size_t length) { input = filename; }
