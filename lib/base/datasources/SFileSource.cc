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

SFileSource::SFileSource() : SDataSource()
    , subevent(0x0000), input(), istream(), buffer_size(0)
{
}

bool SFileSource::open()
{
    printf("File name = |%s|\n", input.c_str());
    istream.open(input.c_str(), std::ios::binary);
    if (!istream.is_open()) {
        std::cerr << "##### Error in SFileSource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

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
        for (auto & unp : unpackers)
        {
            bool res = unp.second->init();
            if (!res) {
                printf("Unpacker %#x not initalized\n", unp.first);
                abort();
            }
        }
    }
    return true;
}

bool SFileSource::close()
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

bool SFileSource::readCurrentEvent()
{
    if (unpackers.size() == 0)
        return false;

    void * buffer[buffer_size];
    istream.read((char*)&buffer, buffer_size);
    bool flag = istream.good();

    if (!flag)
        return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->execute(0, 0, subevent, buffer, buffer_size);
    }
    else
    {
        for (auto & unp : unpackers)
            unp.second->execute(0, 0, unp.first, buffer, buffer_size);
    }

    return true;
}

/**
 * Input for the source.
 *
 * \param filename file name
 * \param buffer_size sizeof the data chunk in the file
 */
void SFileSource::setInput(const std::string& filename, size_t buffer_size) {
    input = filename;
    this->buffer_size = buffer_size;
}
