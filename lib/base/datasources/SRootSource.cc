// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SRootSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <map>

SRootSource::SRootSource() : SDataSource(), buffer_size(0)
{
}

bool SRootSource::open()
{printf("File name = |%s|\n", input.c_str());
    istream.open(input.c_str(), std::ios::binary);
    if (!istream.is_open()) {
        std::cerr << "##### Error in SRootSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    if (unpackers.size() == 0)
        return false;

    if (special_addr != 0x0000)
    {
        bool res = unpackers[special_addr]->init();
        if (!res)
            abort();
    }
    else
    {
        std::map<uint16_t, SUnpacker *>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
        {
            bool res = iter->second->init();
            if (!res)
                abort();
        }
    }
    return true;
}

bool SRootSource::close()
{
    printf("%d Unpackers legth: %d\n", __LINE__, unpackers.size());
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

bool SRootSource::readCurrentEvent()
{
    if (unpackers.size() == 0)
        return false;

    void * buffer[buffer_size];
    istream.read((char*)&buffer, buffer_size);
    bool flag = istream.good();

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

    if (flag)
        return false;

    return true;
}
