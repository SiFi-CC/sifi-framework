// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SROOTSOURCE_H
#define SROOTSOURCE_H

#include "SDataSource.h"

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

#include <cstddef>
#include <string>
#include <fstream>

class SRootSource : public SDataSource
{
public:
    SRootSource();

    virtual bool open();
    virtual bool close();
    virtual bool readCurrentEvent();

    virtual void setInput(const std::string & i, size_t buffer)
    {
        input = i;
        buffer_size = buffer;
    }

private:
    std::string input;
    std::ifstream istream;
    size_t buffer_size;
};

#endif /* SROOTSOURCE_H */
