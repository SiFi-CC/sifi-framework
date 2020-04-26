// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKLOOKUP_H
#define SFIBERSSTACKLOOKUP_H

#include "SLookup.h"

#include <stdint.h>
#include <cstdio>
#include <cassert>

#include <inttypes.h>

struct SLookupChannel;
class SLookupTable;

struct SFibersStackChannel : public SLookupChannel
{
    char side;
    virtual uint read(const char * address) override;
    virtual uint write(char * data, size_t n) const override;
    virtual void print(const char * prefix) const override;
    virtual uint64_t quick_hash() const override;
    virtual void fromHash(uint64_t hash) override;
};

class SFibersStackLookupTable : public SLookupTable
{
public:
    SFibersStackLookupTable(const std::string & container, UInt_t addr_min, UInt_t addr_max, UInt_t channels = 49):
        SLookupTable(container, addr_min, addr_max, channels) {}

    // methods
    SLookupChannel * createChannel() const override { return new SFibersStackChannel(); }
};

#endif /* SFIBERSSTACKLOOKUP_H */
