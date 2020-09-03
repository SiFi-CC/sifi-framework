// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDATASOURCE_H
#define SDATASOURCE_H

#include "sifi_export.h"

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

#include <cstddef>
#include <string>
#include <map>

class SUnpacker;

/**
 * \class SDataSource
 * \ingroup lib_base_datasources
 * This source read events from hld file. For each readout subevent address,
 * a respective unpacker is called. If unpacker is missing, the program is
 * aborted. To ignore the address, nullptr object can be passed.
 */
class SIFI_EXPORT SDataSource
{
public:
    SDataSource() : current_event(0) {}
    virtual ~SDataSource();

    /// Open the source
    /// \return success
    virtual bool open() = 0;
    /// Close the source
    /// \return success
    virtual bool close() = 0;
    /// Read evnt from source
    /// \return success
    virtual bool readCurrentEvent() = 0;

    void setCurrentEvent(long i) { current_event = i; }
    long getCurrentEvent() const { return current_event; }

    void addUnpacker(SUnpacker * unpacker, const std::vector<uint16_t> & address);

private:
    ulong current_event;

protected:
    std::map<uint16_t, SUnpacker *> unpackers;  ///< store unpackers and its addresses
};

#endif /* SDATASOURCE_H */
