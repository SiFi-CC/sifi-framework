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

/**

 \class SRunContainer
 \ingroup lib_base_database

Stores basic run information. Each significant files has corresponding run entry. Significancy
is determined by user. Production runs, calibration runs are meant to be important. Test runs may
be neglected.

The run stores following informations:
* #runid -- consectutive, unique run id, corresoinds to database ID
* #run_starttime -- beginning of run taking
* #run_stoptime   -- end of run taking
* #runtype -- type of run, user defined
runtype -- type of run, to be defined by user

*/

SRunContainer::SRunContainer() {}
