// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SCategoryManager.h"
#include "SiFi.h"

#include <iostream>

/** \class SCategoryManager
\ingroup lib_base

*/

SCategory * SCategoryManager::getCategory(SCategory::Cat cat)
{
    return sifi()->getCurrentEvent()->getCategory(cat);
}
