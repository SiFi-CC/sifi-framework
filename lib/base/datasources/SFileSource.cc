// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFileSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <map>

SFileSource::SFileSource() : SDataSource(), buffer_size(0)
{
}

bool SFileSource::open()
{printf("File name = |%s|\n", input.c_str());
    istream.open(input.c_str(), std::ios::binary);
    if (!istream.is_open()) {
        std::cerr << "##### Error in SFileSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    if (unpackers.size() == 0)
        return false;

    if (special_addr != 0x0000)
    {
        bool res = unpackers[special_addr]->init();
        if (!res) {
            printf("Special unpacker 0x%x not initalized\n", special_addr);
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
                printf("Unpacker 0x%x not initalized\n", iter->first);
                abort();
            }
        }
    }
    return true;
}

bool SFileSource::close()
{
    printf("%d Unpackers legth: %lu\n", __LINE__, unpackers.size());
    if (special_addr != 0x0000)
    {
        unpackers[special_addr]->finalize();
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

bool SFileSource::readCurrentEvent()
{
    if (unpackers.size() == 0)
        return false;

    void * buffer[buffer_size];
    istream.read((char*)&buffer, buffer_size);
    bool flag = istream.good();

    if (!flag)
        return false;

    if (special_addr != 0x0000)
    {
        unpackers[special_addr]->execute(0, 0, buffer, buffer_size);
    }
    else
    {
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->execute(0, 0, buffer, buffer_size);
    }

    return true;
}
