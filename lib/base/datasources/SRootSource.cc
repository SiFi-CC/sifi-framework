// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SRootSource.h"
#include "SUnpacker.h"

#include <TChain.h>

SRootSource::SRootSource(const std::string& tree_name) : SDataSource() {
    chain = new TChain(tree_name.c_str());
}

bool SRootSource::open()
{
    return true;
}

bool SRootSource::close()
{
    return true;
}

bool SRootSource::readCurrentEvent()
{
    return true;
}

/**
 * Set input for the source.
 *
 * \param filename input file name
 * \param length length of buffer to read
 */
void SRootSource::addInput(const std::string& filename) {
    chain->Add(filename.c_str());
}
