
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

#include "SContainer.h"

#include <algorithm> // for max
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>

#include <inttypes.h>

/**
 * \class SLookup
\ingroup lib_base

SLookup is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SLookupManager and stored in the
SLookupContainer. The getParam() method reads content of the SLookupContainer and
fills variables inside the SLookup object. The putParam method allows to update
parameters in the container and write to param file.

\sa SFibersCalibratorPar
\sa SFibersDigitizerPar
\sa SFibersGeomPar
*/

/**
 * Parses the mapping part of the lookup entry (see void SLookupTable for more
 * details). The lookup entry must provide all values to initialize relevant
 * members of SLookupChannel. This function must handle all of them. Must return
 * number of read bytes.
 *
 * If sscanf is used, it returns number of read values. The assert() call should
 * test whether the number is correct, and abort execution if not.
 * To get number of read bytes, use `%n` after the last value.
 *
 * \param buffer string containing data to parse
 * \return number of parsed elements.
 */
uint SLookupChannel::read(const char* buffer)
{
    uint n;
    int cnt = sscanf(buffer, "%hu" "%hu" "%hu" "%n", &m, &l, &s, &n);
    assert(cnt == 3);
    return n;
}

/**
 * Exports the parameters to the mapping part of the entry. When overriding
 * this class, the subclass must address all values in the channel. The output
 * format should be consisten with the read format parsing.
 *
 * \param[out] buffer buffer to be written
 * \param n buffer length
 * \return number of written bytes.
 */
uint SLookupChannel::write(char* buffer, size_t n) const
{
    uint cnt = snprintf(buffer, n, "%d %d %d", m, l, s);
    if (cnt < 0) return cnt;
    if (cnt < n) return 0;
    return cnt;
}

/**
 * Prints the parameters to the mapping part of the entry. When overriding
 * this class, the subclass must address all values in the channel. The output
 * format should be consisten with the read format parsing.
 *
 * \param newline puts newline on the end
 * \param prefix a text which should be displayed before the content of the
 * channel params. If prefix is empty, then
 */
void SLookupChannel::print(bool newline, const char* prefix) const
{
    printf("%s %d %d %d", prefix, m, l, s);
    if (newline) putchar('\n');
}

/**
 * In certain cases lookup channels are stored in an unordered_map STL
 * container. To provide quick access a specific hash function must be provides.
 * Here we assumed that hash is a 64 bit word, which stores m, l, s lookup
 * values at respectively 47-40, 55-48 and 63-56 bits of te word. The lower bits
 * can be used in overriding structures to extend their classes. The hash can
 * be also redefined in subclassing structures.
 *
 * The hash should be unike like the address itself is. What more, the hash must
 * be reversible. The fromHash() functions decodes hash to address values.
 *
 * If overriding either of quick_hash() and fromHash(), the second most likely
 * als must be overrided and adjusted.
 *
 * \return hash of the object
 */
uint64_t SLookupChannel::quickHash() const
{
    uint64_t h = (uint64_t)m << 40 | (uint64_t)l << 48 | (uint64_t)s << 56;
    return h;
}

/**
 * Recreates address from hash. See quickHash() for details about hash.
 *
 * \param hash the channel hash
 */
void SLookupChannel::fromHash(uint64_t hash)
{
    m = uint8_t(hash >> 40 & 0xff);
    l = uint8_t(hash >> 48 & 0xff);
    s = uint8_t(hash >> 56 & 0xff);
}

/**
 * Constructor
 *
 * \param addr,nchan address and channels number
 */
SLookupBoard::SLookupBoard(uint addr, uint nchan) : addr(addr), nchan(nchan), channels(nchan) {}

/**
 * Prints all registered channels for given board.
 */
void SLookupBoard::print()
{
    char p[16];
    for (uint i = 0; i < nchan; ++i)
    {
        sprintf(p, "0x%x  %2d  ", addr, i);
        if (channels[i]) channels[i]->print(true, p);
    }
}

/**
 * Initialize lookup table from container named #container for given address
 * range of \p addr_min, \p addr_max. Defines that each board has no more channeel
 * than specified in #channels.
 *
 * \param name container name
 * \param addr_min,addr_max lower and upper range of boards addresses
 * \param channels maximal number of channels in each board
 */
SLookupTable::SLookupTable(const std::string& name, uint addr_min, uint addr_max, uint channels)
    : container(name), a_min(addr_min), a_max(addr_max), channels(channels), is_init(false),
      boards(addr_max - addr_min + 1)
{
}

/**
 * Initialize lookup table from the container. The container must exists
 * otherwise exception is thrown.
 *
 * \param sc container object
 */
void SLookupTable::fromContainer(SContainer* sc)
{
    if (!sc) throw "No lookup container.";

    const std::vector<std::string>& lv = sc->lines;

    for (auto line : lv)
    {
        uint addr, chan, len;
        sscanf(line.c_str(), "%x %d%n", &addr, &chan, &len);
        if (addr < a_min or addr > a_max)
        {
            std::cerr << "Can't add board " << addr << " inside (0x" << std::hex << a_min << ", 0x"
                      << a_max << std::dec << "), skipping." << std::endl;
            continue;
        }
        uint idx = addr - a_min;
        if (boards[idx] == nullptr) boards[idx] = std::make_unique<SLookupBoard>(addr, channels);

        boards[idx]->setChannel(chan, std::unique_ptr<SLookupChannel>(createChannel()));
        boards[idx]->getChannel(chan)->read(line.c_str() + len);
    }

    is_init = true;
}

/**
 * Export lookup table to the container. The function is called by the
 * SDatabase class. The container must be registered in the Parameters
 * Manager. Otherwise an exception is thrown.
 *
 * \param sc container object
 */
void SLookupTable::toContainer(SContainer* sc) const
{
    if (!sc) throw "No lookup container.";

    sc->lines.clear();

    const int len = 1024;
    char buff[len];

    for (uint addr = a_min; addr < a_max; ++addr)
    {
        uint idx = addr - a_min;
        if (boards[idx])
        {
            for (uint c = 0; c < channels; ++c)
            {
                SLookupChannel* chan = boards[idx]->getChannel(c);
                if (chan)
                {
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

/**
 * Return lookup channel object based on the board address and channel
 * number. Interally calls SLookupBoard::getChannel(). See its documentation
 * for further information.
 *
 * \param addr board address
 * \param chan channel number
 * \return channel object or nullptr if boad or channel doesn't exists.
 */
SLookupChannel* SLookupTable::getAddress(uint addr, uint chan)
{
    if (addr > a_max) return nullptr;
    if (addr < a_min) return nullptr;

    if (!boards[addr - a_min]) return nullptr;
    return boards[addr - a_min]->getChannel(chan);
}

/**
 * Prints all existing boards and its channel lookup tables
 */
void SLookupTable::print()
{
    printf("[%s]\n", container.c_str());
    size_t nboards = a_max - a_min + 1;
    for (uint i = 0; i < nboards; ++i)
        if (boards[i]) boards[i]->print();
}
