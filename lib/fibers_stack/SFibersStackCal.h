// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCAL_H
#define SFIBERSSTACKCAL_H

#include "sifi_export.h"

#include "SFibersStackRaw.h"

#include <Rtypes.h>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersStackCal : public SFibersStackRaw
{
  public:
    // constructors
    SFibersStackCal() = default;
    virtual ~SFibersStackCal() = default;

    ClassDef(SFibersStackCal, 1); // container for fibers stack calibrated data
};

#endif /* SFIBERSSTACKCAL_H */
