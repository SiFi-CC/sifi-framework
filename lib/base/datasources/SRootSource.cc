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

#include <iostream>
#include <map>

SRootSource::SRootSource() : SDataSource()
{
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
