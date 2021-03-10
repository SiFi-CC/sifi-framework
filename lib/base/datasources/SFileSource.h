// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFILESOURCE_H
#define SFILESOURCE_H

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
 * File source reads data from text or binary file. As the subevent id is not
 * provided by regular text or binary file, the unpacker must by provied with
 * forceUnpacker() function. When specifying the input file, one needs to
 * specify also size of buffer to be read from file. The source expect the data
 * chunks of equal sizes.
 */
class SFileSource : public SDataSource
{
  public:
    SFileSource();

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string& filename, size_t buffer_size);

    /// Define unpacker address for the source
    /// \param address emulated subevent id
    virtual void forceUnpacker(uint16_t address) { subevent = address; }

  private:
    uint16_t subevent;     ///< set address for unpacker
    std::string input;     ///< file input
    std::ifstream istream; ///< file reader stream
    size_t buffer_size;    ///< size of data chunk
};

#endif /* SFILESOURCE_H */
