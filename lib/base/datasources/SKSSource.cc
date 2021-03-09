
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
#include "SFibersStackDDUnpacker.h"
#include "SUnpacker.h"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SKSSource::SKSSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), istream(), buffer_size(0)
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
    if (!istream.is_open())
    {
        std::cerr << "Error in SKSSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    std::string tmp;
    samples = 0;

    istream.seekg(std::ios_base::beg); // getting length of the signal
    istream >> tmp >> tmp >> samples;

    getline(istream, tmp); // getting max channels
    getline(istream, tmp);

    std::string max_channel_str = tmp.substr(tmp.length() - 1, tmp.length());
    max_channels = std::stoi(max_channel_str);
    channel = subevent & 0x0f; // getting channel number

    if (channel > max_channels)
    {
        std::cerr << "Error in SKSSource::open()! Incorrect channel number!" << std::endl;
        std::cerr << "channel = " << channel << "\t max_channel = " << max_channels << std::endl;
        return false;
    }

    getline(istream, tmp); // reading remaining line before data starts

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
        auto ddUnp = dynamic_cast<SFibersStackDDUnpacker*>(unpackers[subevent]);
        if (ddUnp) ddUnp->setDataLen(samples);
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
            auto ddUnp = dynamic_cast<SFibersStackDDUnpacker*>(iter->second);
            if (ddUnp) ddUnp->setDataLen(samples);
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
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->finalize();
    }

    istream.close();
    return true;
}

bool SKSSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;

    std::string csvLine;
    std::string csvElement;
    std::vector<std::string> csvRow;

    float time_start;
    float time_stop;

    float* buffer = new float[samples];

    Int_t nSamples = 0;

    for (nSamples = 0; nSamples < samples; nSamples++)
    {
        csvRow.clear();
        getline(istream, csvLine);
        if (istream.eof()) break;

        std::stringstream stream(csvLine);
        while (getline(stream, csvElement, ','))
        {
            csvRow.push_back(csvElement);
        }

        if (nSamples == 0) time_start = std::stof(csvRow[0]);
        if (nSamples == samples - 1) time_stop = std::stof(csvRow[0]);

        buffer[nSamples] = std::stof(csvRow.at(channel + 1));
    }

    float time_bin = 1e9 * (time_stop - time_start) / (samples - 1); // ns

    bool flag = istream.good();

    if (!flag or nSamples < samples) return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setSampleTons(time_bin);
        unpackers[subevent]->setADCTomV(1.);
        // TODO must pass event number to the execute
        unpackers[subevent]->execute(0, 0, subevent, buffer, nSamples * sizeof(buffer[0]));
    }
    else
    {
        for (const auto& u : unpackers)
        {
            u.second->setSampleTons(time_bin);
            u.second->setADCTomV(1.);
            u.second->execute(0, 0, u.first, buffer, nSamples * sizeof(buffer[0]));
        }
    }

    return true;
}

/**
 * Set input for the source.
 *
 * \param filename input file name
 */
void SKSSource::setInput(const std::string& filename) { input = filename; }
