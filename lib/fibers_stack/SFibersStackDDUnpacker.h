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

#include "SDDUnpacker.h"
#include "SLookup.h"

#include <string>

class SCategory;
class SFibersStackGeomPar;
class SFibersStackDDUnpackerPar;
class SFibersStackLookupTable;

class SFibersStackDDUnpacker : public SDDUnpacker
{
protected:
    // members
    SCategory * catDDSamples;           ///< fibers raw category
    SCategory * catFibersRaw;           ///< fibers raw category
    SFibersStackDDUnpackerPar * pDDUnpackerPar; ///< calibrator parameters
    SFibersStackLookupTable * pLookUp;

public:
    // constructor
    SFibersStackDDUnpacker();
    // destructor
    virtual ~SFibersStackDDUnpacker();

    virtual bool init() override;
protected:
    // methods
    bool decode(uint16_t address, float * data, size_t length) override;

private:
    std::string lookup_name;
    SLookupTable * lookup_table;
};

#endif /* SFIBERSSTACKUNPACKER_H */