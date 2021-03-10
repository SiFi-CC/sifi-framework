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

#include "sifi_export.h"

#include "SLookup.h"

#include <cassert>
#include <cstdio>
#include <stdint.h>

#include <inttypes.h>

struct SLookupChannel;
class SLookupTable;

/**
 * Extends the SLookupChannel. In addiotion to `m`, `l`, and `s` adds #side
 * dimension, which must be equal either to `l` (left) or `r` (right) denoting
 * side of the read fiber.
 */
struct SIFI_EXPORT SFibersStackChannel : public SLookupChannel
{
    char side; ///< side of a fiber, either 'l' or 'r'

    virtual uint read(const char* buffer) override;
    virtual uint write(char* buffer, size_t n) const override;
    virtual void print(bool newline = true, const char* prefix = "") const override;
    virtual uint64_t quickHash() const override;
    virtual void fromHash(uint64_t hash) override;
};

class SIFI_EXPORT SFibersStackLookupTable : public SLookupTable
{
public:
    using SLookupTable::SLookupTable;

    // methods
    SLookupChannel* createChannel() const override { return new SFibersStackChannel(); }
};

#endif /* SFIBERSSTACKLOOKUP_H */
