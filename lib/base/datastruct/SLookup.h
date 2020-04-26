// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SLOOKUP_H
#define SLOOKUP_H

#include "SParManager.h"

#include <Rtypes.h>

#include <stdint.h>

class SLookupTable;

struct SLookupChannel
{
    uint8_t m, l, s;
    virtual uint read(const char * data);
    virtual uint write(char * data, size_t n) const;
    virtual void print(const char * prefix = 0) const;
    virtual uint64_t quick_hash() const;
    virtual void fromHash(uint64_t hash);
};

class SLookupBoard
{
private:
    UInt_t addr, nchan;
    SLookupChannel ** channels;

public:
    SLookupBoard(UInt_t addr, UInt_t nchan);
    SLookupBoard(const SLookupBoard &) = delete;
    SLookupBoard & operator=(const SLookupBoard &) = delete;

    virtual ~SLookupBoard();

    void setChannel(UInt_t chan, SLookupChannel * c) { channels[chan] = c; }
    SLookupChannel * getChannel(UInt_t chan) { return channels[chan]; }

    virtual void print();
};

class SLookupTable
{
protected:
    std::string container;
    uint a_min, a_max, channels;
    bool is_init;

    SLookupBoard ** boards;

public:
    // constructor
    SLookupTable(const std::string & container, UInt_t addr_min, UInt_t addr_max, UInt_t channels = 49);
    SLookupTable(const SLookupTable &) = delete;
    SLookupTable & operator=(const SLookupTable &) = delete;

    // destructor
    virtual ~SLookupTable();

    virtual SLookupChannel * createChannel() const { return new SLookupChannel; }

    SLookupChannel * getAddress(UInt_t addr, UInt_t chan) {
        if (!is_init) fromContainer();
        return boards[addr-a_min]->getChannel(chan);
    }

    virtual void print();

protected:
    void fromContainer();
    void toContainer() const;
    friend void SParManager::writeContainers(const std::vector<std::string> & conts) const;
};

#endif /* SLOOKUP_H */
