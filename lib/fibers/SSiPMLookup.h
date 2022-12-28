// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SSIPMLOOKUP_H
#define SSIPMLOOKUP_H

#include "sifi_export.h"

#include "SLookup.h"

#include <cstdio>
#include <memory>
#include <stdint.h>
#include <sys/types.h>

struct SIFI_EXPORT SSiPMChannel : public SLookupChannel
{
    SSiPMChannel() {};
    virtual ~SSiPMChannel() {};
};

class SIFI_EXPORT SSiPMLookupTable : public SLookupTable
{
    
public:
    using SLookupTable::SLookupTable;
    virtual ~SSiPMLookupTable() = default;
    
    virtual std::unique_ptr<SLookupChannel> createChannel() const override
    {
        return std::make_unique<SSiPMChannel>();
    }
    
    int getModules();
    int getLayers(int m);
    int getSiPMsPerLayer(int m, int l);
    
};

#endif /* SSIPMLOOKUP_H */
