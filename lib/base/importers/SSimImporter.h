// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef SSIMIMPORTER_H
#define SSIMIMPORTER_H

#include "sifi_export.h"

#include "SUnpacker.h"
#include "SFibersLookup.h"

#include <cstddef>     // for size_t
#include <cstdint>     // for uint16_t
#include <sys/types.h> // for ulong

class SCategory;
class SFibersGeomPar;
class SDetectorGeomPar;

class SIFI_EXPORT SSimImporter : public SUnpacker
{
protected:
    // members
    SCategory* catSiPMHit{nullptr}; ///< raw SiPM hit category
    SDetectorGeomPar* pDetGeomPar{nullptr}; ///< detector geometry

public:
    // constructor
    SSimImporter();
    // destructor
    virtual ~SSimImporter() = default;

    virtual bool init() override;

    virtual bool execute(ulong event, ulong seq_number, uint16_t subevent, void* buffer,
                         size_t length) override;
};

#endif /* SSIMSIMPORTER_H */
