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
#include "SEvent.h"
#include "SiFi.h"

/**

\ingroup lib_base_core

*/

namespace SCategoryManager
{

SCategory* getCategory(SCategory::Cat cat) { return sifi()->getCurrentEvent()->getCategory(cat); }

}; // namespace SCategoryManager
