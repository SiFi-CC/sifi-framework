
// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SKSSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>

static Int_t static_var = 5;
static Int_t nSamples;

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SKSSource::SKSSource(uint16_t subevent) : SDataSource()
    , subevent(subevent), input(), istream(), buffer_size(0)
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SKSSource::open()
{
    istream.open(input.c_str(), std::ios::in);
    if (!istream.is_open()) {
        std::cerr << "##### Error in SKSSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }
   
   samples = getNSamples();
   
   std::string line;
   
   for(int i=0; i<3; ++i)
       getline(istream, line);
   
    if (unpackers.size() == 0)
        return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
    
        bool res = unpackers[subevent]->init();
        if (!res) {
            printf("Forced unpacker %#x not initalized\n", subevent);
            abort();
        }
    }
    else
    {
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
        {
            bool res = iter->second->init();
            if (!res) {
                printf("Unpacker %#x not initalized\n", iter->first);
                abort();
            }
        }
    }
    return true;
}

bool SKSSource::close()
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
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->finalize();
    }

    istream.close();
    return true;
}

bool SKSSource::readCurrentEvent()
{
    if (unpackers.size() == 0)
    return false;
       
    std::string csvLine;    
    std::string csvElement;
    std::vector <std::string> csvRow;
    
    float time_start;
    float time_stop;
    
    float* buffer = new float [samples];

    for (Int_t i = 0; i < samples; i++)
    {
        csvRow.clear();
        getline(istream, csvLine);
        std::stringstream stream(csvLine);
        while(getline(stream, csvElement, ','))
        {
            csvRow.push_back(csvElement);
        }
        
        if (i == 0) 
            time_start = std::stof(csvRow[0]);
        if (i == samples-1) 
            time_stop = std::stof(csvRow[0]);
        
        buffer[i] = std::stof(csvRow.at(1)); //TODO only channel 0 digitized at the moment
    }
    
    
    float time_bin = 1e9*(time_stop-time_start)/(samples-1); //ns
    buffer_size = samples;
    
    bool flag = istream.good();

    if (!flag)
        return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setTimeBin(time_bin);
        unpackers[subevent]->setADCtomV(1.);
        // TODO must pass event number to the execute
        unpackers[subevent]->execute(0, 0, subevent, buffer, buffer_size * sizeof(buffer[0]));
    }
    else
    {
        for (const auto & u : unpackers)
        {
            u.second->setTimeBin(time_bin);
            u.second->setADCtomV(1.);
            u.second->execute(0, 0, u.first, buffer, buffer_size * sizeof(buffer[0]));
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
void SKSSource::setInput(const std::string& filename, size_t length) {
    input = filename;
    buffer_size = length;
}

int SKSSource::getNSamples()
{
    std::string tmp;
    int nSamples = 0;
    
    if (!istream.good())
    {
        std::cerr << "##### Error in SKSSource::getNSamples()! Input file not opened!" << std::endl;
    }
    
    istream.seekg(std::ios_base::beg);
    istream >> tmp >> tmp >> nSamples;

    return nSamples;
}
