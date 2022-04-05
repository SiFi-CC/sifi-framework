// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSHLDUNPACKER_H
#define SFIBERSHLDUNPACKER_H

#include "sifi_export.h"

#include "SLookup.h"
#include "SUnpacker.h"

#include <string>

class SCategory;
class SFibersGeomPar;
class SFibersLookupTable;

class SIFI_EXPORT SFibersHLDUnpacker : public SUnpacker
{

protected:
    SCategory* catFibersRaw;
    SCategory* catFibersCal;
    SFibersLookupTable* pLookUp;

public:
    SFibersHLDUnpacker();

    virtual bool init() override;

    virtual bool execute(ulong event, ulong seq_number, uint16_t subevent, void* buffer,
                         size_t length) override;
};

#endif
