// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SKSSOURCE_H
#define SKSSOURCE_H

#include "sifi_export.h"

#include "SDataSource.h"

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

#include <cstddef>
#include <fstream>
#include <string>

/**
 * Extends SDataSOurce to read data from the oscilloscope.
 */
class SIFI_EXPORT SKSSource : public SDataSource
{
public:
    explicit SKSSource(uint16_t subevent);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string& filename);

private:
    int samples;
    int max_channels;
    uint8_t channel;
    uint16_t subevent;     ///< subevent id
    std::string input;     ///< source file name
    std::ifstream istream; ///< input file stream
    size_t buffer_size;    ///< data buffer size
};
#endif /* SKSSOURCE_H */
