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

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

#include <cstddef>
#include <string>
#include <map>

class SUnpacker;

class SDataSource
{
public:
    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool readCurrentEvent() = 0;

    void addUnpacker(SUnpacker * unpacker, uint16_t address = 0x0000);

private:

protected:
    std::map<uint16_t, SUnpacker *> unpackers;
    uint16_t special_addr;
};

#endif /* SDATASOURCE_H */
