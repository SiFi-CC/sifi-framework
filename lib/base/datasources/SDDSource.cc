// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SDDSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <map>

SDDSource::SDDSource(uint16_t address) : SDataSource()
    , unpacker(address), input(), istream(), buffer_size(0)
{
}

bool SDDSource::open()
{
    printf("File name = |%s|\n", input.c_str());
    istream.open(input.c_str(), std::ios::binary);
    if (!istream.is_open()) {
        std::cerr << "##### Error in SDDSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    if (unpackers.size() == 0)
        return false;

    if (unpacker != 0x0000)
    {
        if (!unpackers[unpacker]) abort();
    
        bool res = unpackers[unpacker]->init();
        if (!res) {
            printf("Forced unpacker %#x not initalized\n", unpacker);
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

bool SDDSource::close()
{
    if (unpacker != 0x0000)
    {
        if (unpackers[unpacker])
            unpackers[unpacker]->finalize();
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

bool SDDSource::readCurrentEvent()
{
    if (unpackers.size() == 0)
        return false;

    void * buffer[buffer_size];
    istream.read((char*)&buffer, buffer_size);
    bool flag = istream.good();

    if (!flag)
        return false;

    if (unpacker != 0x0000)
    {
        if (!unpackers[unpacker]) abort();
        unpackers[unpacker]->execute(0, 0, unpacker, buffer, buffer_size);
    }
    else
    {
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->execute(0, 0, iter->first, buffer, buffer_size);
    }

    return true;
}

void SDDSource::setInput(const std::string& i, size_t buffer) {
    input = i;
    buffer_size = buffer;
}
