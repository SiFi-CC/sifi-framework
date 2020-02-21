// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <cstdio>
#include <iostream>

#include "SLookupManager.h"
#include "SLookupContainer.h"
#include "SLookup.h"

/** \class SLookup
\ingroup lib_base

SLookup is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SLookupManager and stored in the
SLookupContainer. The getParam() method reads content of the SLookupContainer and
fills variables inside the SLookup object. The putParam method allows to update
parameters in the container and write to param file.

\sa MFibersStackCalibratorPar
\sa MFibersStackDigitizerPar
\sa MFibersStackGeomPar
*/

void SLookupChannel::setAddress(const char* address)
{
    sscanf(address, "%*s %*s %d %d %d\n", &m, &l, &s);
}

void SLookupChannel::print(const char * prefix)
{
    printf("%s %d %d %d\n", prefix, m, l, s);
}

SLookupBoard::SLookupBoard(UInt_t addr, UInt_t nchan) :
    addr(addr), nchan(nchan)
{
    channels = new SLookupChannel*[nchan];
    for (uint i = 0; i < nchan; ++i)
        channels[i] = 0;
}

SLookupBoard::~SLookupBoard()
{
    delete [] channels;
}

void SLookupBoard::print()
{
    char p[16];
    for (uint i = 0; i < nchan; ++i)
    {
        sprintf(p, "0x%x  %2d  ", addr, i);
        channels[i]->print(p);
    }
}


SLookupTable::SLookupTable(const std::string& container, UInt_t addr_min, UInt_t addr_max, UInt_t channels) :
    container(container), a_min(addr_min), a_max(addr_max), channels(channels), is_init(false)
{
    size_t nboards = addr_max - addr_min + 1;
    boards = new SLookupBoard*[nboards];
    memset(boards, 0, sizeof(SLookupBoard*) * nboards);
}

void SLookupTable::init()
{
    SLookupContainer * lc = lm()->getLookupContainer(container);

    const LookupVector & lv = lc->getLines();

    size_t n = lv.size();
    for (uint i = 0; i < n; ++i)
    {
        uint addr, chan;
        char address[1024];
        sscanf(lv[i].c_str(), "%x %x %s", &addr, &chan, address);
        if (addr < a_min or addr > a_max)
        {
            std::cerr << "Can't add board " << addr << " inside (0x" << std::hex << a_min << ", 0x" << a_max << std::dec << "), skipping." << std::endl;
            continue;
        }
        uint idx = addr - a_min;
        if (boards[idx] == 0)
        {
            boards[idx] = new SLookupBoard(addr, channels);
            for (int i = 0; i < channels; ++i)
                boards[idx]->setChannel(i, initial());
        }

        boards[idx]->setAddress(chan, lv[i].c_str());
    }

    is_init = true;
}

SLookupTable::~SLookupTable()
{
    delete [] boards;
}

void SLookupTable::print()
{
    if (!is_init) init();

    printf("[%s]\n", container.c_str());
    size_t nboards = a_max - a_min + 1;
    for (uint i = 0; i < nboards; ++i)
        if (boards[i]) boards[i]->print();
}
