// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersLookup.h"

/**
 * \class SFibersLookupTable
\ingroup lib_fibers

A unpacker task.

\sa STask
*/

uint SFibersChannel::read(const char* buffer)
{
    uint n;
    int cnt = sscanf(buffer, "%2" SCNu8 "%2" SCNu8 "%2" SCNu8 "%*[ ]%c %n", &m, &l, &s, &side, &n);
    assert(cnt == 4);
    return n;
}

uint SFibersChannel::write(char* buffer, size_t n) const
{
    uint cnt = snprintf(buffer, n, "%3d  %3d  %3d   %c", m, l, s, side);
    if (cnt < 0) return cnt;
    if (cnt < n) return 0;
    return cnt;
}

void SFibersChannel::print(bool newline, const char* prefix) const
{
    printf("%s %d  %d  %d  %c", prefix, m, l, s, side);
    if (newline) putchar('\n');
}

uint64_t SFibersChannel::quickHash() const
{
    return SLookupChannel::quickHash() | (uint64_t)side << 32;
}

void SFibersChannel::fromHash(uint64_t hash)
{
    SLookupChannel::fromHash(hash);
    side = hash >> 32 & 0xff;
}
