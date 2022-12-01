// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSTP4TO1UNPACKER_H
#define SFIBERSTP4TO1UNPACKER_H

#include "sifi_export.h"

#include "SUnpacker.h"

#include <cstddef>     // for size_t
#include <cstdint>     // for uint16_t
#include <sys/types.h> // for ulong

class SCategory;
class SFibersLookupTable;

class SIFI_EXPORT SFibersTP4to1Unpacker : public SUnpacker
{

protected:
    SCategory* catFibersRaw;
    SCategory* catFibersCal;

public:
    SFibersTP4to1Unpacker();

    virtual bool init() override;

    virtual bool execute(ulong event, ulong seq_number, uint16_t subevent, void* buffer,
                         size_t length) override;
};

#endif

