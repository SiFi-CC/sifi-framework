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

#include "sifi_export.h"

#include "SDataSource.h"

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

#include <cstddef>
#include <string>
#include <fstream>

/**
 * Read data from root file. needs better implementatoin when a such data show
 * up.
 * \todo Implement actually
 */
class SRootSource : public SDataSource
{
public:
    SRootSource();

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;

private:
    std::string input;      ///< root file input
};

#endif /* SROOTSOURCE_H */
