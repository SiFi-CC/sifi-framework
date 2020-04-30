// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCATEGORYMANAGER_H
#define SCATEGORYMANAGER_H

#include "sifi_export.h"

#include "SCategory.h"

namespace SCategoryManager
{
   SIFI_EXPORT SCategory * getCategory(SCategory::Cat cat);
}

#endif /* STASKMANAGER_H */
