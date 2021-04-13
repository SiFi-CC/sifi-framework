// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSDDUNPACKER_H
#define SFIBERSDDUNPACKER_H

#include "sifi_export.h"

#include "SDDUnpacker.h"

#include <cstddef> // for size_t
#include <cstdint> // for uint16_t

class SCategory;
class SFibersDDUnpackerPar;
class SFibersDDCalibratorPar;
class SFibersLookupTable;

class SIFI_EXPORT SFibersDDUnpacker : public SDDUnpacker
{
protected:
    // members
    SCategory* catDDSamples;              ///< fibers raw category
    SCategory* catFibersRaw;              ///< fibers raw category
    SFibersDDUnpackerPar* pDDUnpackerPar; ///< unpacker parameters
    SFibersDDCalibratorPar* pDDCalPar;    ///< calibrator parameters
    SFibersLookupTable* pLookUp;          ///< lookup table

public:
    // constructor
    SFibersDDUnpacker();
    // destructor
    virtual ~SFibersDDUnpacker() = default;

    virtual bool init() override;

protected:
    // methods
    bool decode(uint16_t subevtid, float* data, size_t length) override;
};

#endif /* SFIBERSUNPACKER_H */
