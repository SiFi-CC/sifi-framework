// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SRunContainer.h"

#include <iomanip>
#include <iostream>

/**

 \class SRunContainer
 \ingroup lib_base_database

Stores basic run information. Each significant files has corresponding run entry. Significancy
is determined by user. Production runs, calibration runs are meant to be important. Test runs may
be neglected.

The run stores following informations:
* #runid -- consectutive, unique run id, corresoinds to database ID
* #start_time -- beginning of run taking
* #stop_time   -- end of run taking
* #runtype -- type of run, user defined
runtype -- type of run, to be defined by user

*/

void SRunContainer::print() const
{
    std::cout << "=== Run info === id: " << runid << "  type: " << runtype
              << "  start: " << std::put_time(gmtime(&start_time), "%F %T %Z")
              << "  stop: " << std::put_time(gmtime(&stop_time), "%F %T %Z") << std::endl;
}
