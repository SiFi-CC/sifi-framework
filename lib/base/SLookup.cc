// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SLookup.h"
#include "SParManager.h"

#include <cstdio>
#include <iostream>
#include <cassert>

#include <inttypes.h>

/** \class SLookup
\ingroup lib_base

SLookup is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SLookupManager and stored in the
SLookupContainer. The getParam() method reads content of the SLookupContainer and
fills variables inside the SLookup object. The putParam method allows to update
parameters in the container and write to param file.

\sa SFibersStackCalibratorPar
\sa SFibersStackDigitizerPar
\sa SFibersStackGeomPar
*/

uint SLookupChannel::read(const char* data)
{
    uint n;
    int cnt = sscanf(data, "%2" SCNu8 "%2" SCNu8 "%2" SCNu8 "%n", &m, &l, &s, &n);
    assert(cnt == 3);
    return n;
}

uint SLookupChannel::write(char* data, size_t n) const
{
    uint cnt = snprintf(data, n, "%d %d %d", m, l, s);
    if (cnt < 0) return cnt;
    if (cnt < n) return 0;
    return cnt;
}

void SLookupChannel::print(const char * prefix) const
{
    printf("%s %d %d %d", prefix, m, l, s);
    if (prefix[0] != '\0') putchar('\n');
}

uint64_t SLookupChannel::quick_hash() const
{
    uint64_t h = (uint64_t)m << 40 | (uint64_t)l << 48 | (uint64_t)s << 56;
    return h;
}

void SLookupChannel::fromHash(uint64_t hash)
{
    m = uint8_t(hash >> 40 & 0xff);
    l = uint8_t(hash >> 48 & 0xff);
    s = uint8_t(hash >> 56 & 0xff);
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
        if (channels[i]) channels[i]->print(p);
    }
}


SLookupTable::SLookupTable(const std::string& container, UInt_t addr_min, UInt_t addr_max, UInt_t channels) :
    container(container), a_min(addr_min), a_max(addr_max), channels(channels), is_init(false)
{
    size_t nboards = addr_max - addr_min + 1;
    boards = new SLookupBoard*[nboards];
    memset(boards, 0, sizeof(SLookupBoard*) * nboards);
}

void SLookupTable::fromContainer()
{
    SContainer * lc = pm()->getContainer(container);
    if (!lc) throw "No lookup container.";

    const std::vector<std::string> & lv = lc->lines;

    for (auto line: lv)
    {
        uint addr, chan, len;
        sscanf(line.c_str(), "%x %d%n", &addr, &chan, &len);
        if (addr < a_min or addr > a_max)
        {
            std::cerr << "Can't add board " << addr << " inside (0x" << std::hex << a_min << ", 0x" << a_max << std::dec << "), skipping." << std::endl;
            continue;
        }
        uint idx = addr - a_min;
        if (boards[idx] == 0)
            boards[idx] = new SLookupBoard(addr, channels);

        boards[idx]->setChannel(chan, createChannel());
        boards[idx]->getChannel(chan)->read(line.c_str() + len);
    }

    is_init = true;
}

void SLookupTable::toContainer() const
{
    SContainer * sc = pm()->getContainer(container);
    if (!sc) throw "No lookup container.";

    sc->lines.clear();

    const int len = 1024;
    char buff[len];

    for (uint addr = a_min; addr < a_max; ++addr) {
        uint idx = addr - a_min;
        if (boards[idx])
        {
            for (uint c = 0; c < channels; ++c) {
                SLookupChannel * chan = boards[idx]->getChannel(c);
                if (chan) {
                    snprintf(buff, len, "  %#x  %3d\t\t", addr, c);
                    std::string s;
                    s.reserve(128);
                    s += buff;
                    chan->write(buff, len);
                    s += buff;
                    sc->lines.push_back(s);
                }
            }
        }
    }
}

SLookupTable::~SLookupTable()
{
    delete [] boards;
}

void SLookupTable::print()
{
    if (!is_init) fromContainer();

    printf("[%s]\n", container.c_str());
    size_t nboards = a_max - a_min + 1;
    for (uint i = 0; i < nboards; ++i)
        if (boards[i]) boards[i]->print();
}
