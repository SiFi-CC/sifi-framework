// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKDDUNPACKER_H
#define SFIBERSSTACKDDUNPACKER_H

#include "sifi_export.h"

#include "SDRSource.h"
#include "SLookup.h"
#include "SUnpacker.h"

#include <string>

class SCategory;
class SFibersStackGeomPar;
class SFibersStackLookupTable;

class SIFI_EXPORT SSiFiCCDetResImporter : public SUnpacker
{
protected:
    // members
    SCategory * catFibersCal;                   ///< fibers raw category
    SFibersStackLookupTable * pLookUp;          ///< lookup table

public:
    // constructor
    SSiFiCCDetResImporter();
    // destructor
    virtual ~SSiFiCCDetResImporter() = default;

    virtual bool init() override;

    virtual bool execute(ulong event, ulong seq_number, uint16_t subevent,
        void * buffer, size_t length) override;
};

#endif /* SFIBERSSTACKUNPACKER_H */
