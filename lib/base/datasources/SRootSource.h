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

#include <TChain.h>

#include <string>

/**
 * Read data from root file. needs better implementatoin when a such data show
 * up.
 * \todo Implement actually
 */
class SIFI_EXPORT SRootSource : public SDataSource
{
  public:
    SRootSource(const std::string& tree_name);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void addInput(const std::string& filename);

    virtual long getEntries() { return chain->GetEntries(); }

  protected:
    TChain* chain;
};

#endif /* SROOTSOURCE_H */
