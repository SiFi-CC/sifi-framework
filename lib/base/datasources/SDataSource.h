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

#include <cstdint> // for uint16_t
#include <map>
#include <sys/types.h> // for ulong
#include <vector>      // for vector

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

    /// Set index of the current event
    /// \param i new index of the current event
    void setCurrentEvent(long i) { current_event = i; }
    /// Get index of the current event
    /// \return current event index
    long getCurrentEvent() const { return current_event; }

    void addUnpacker(SUnpacker* unpacker, const std::vector<uint16_t>& address);

private:
    ulong current_event; ///< current event index

protected:
    std::map<uint16_t, SUnpacker*> unpackers; ///< store unpackers and its addresses
};

#endif /* SDATASOURCE_H */
