// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SMysqlInterface.h"
#include "SCalContainer.h"
#include "SLookup.h"
#include "SPar.h"
#include "SParContainer.h"
#include "SParSource.h"
#include "SiFi.h"

#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <iostream>
#include <utility> // for pair, make_pair, move

/**
 * \class SMysqlInterface

\ingroup lib_base

*/
