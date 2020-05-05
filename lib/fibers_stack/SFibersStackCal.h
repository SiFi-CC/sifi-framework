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

#include <SFibersStackRaw.h>

class SFibersStackCal : public SFibersStackRaw
{
public:
    // constructors
    SFibersStackCal();
    virtual ~SFibersStackCal() = default;

    ClassDef(SFibersStackCal, 1);   // container for fibers stack calibrated data
};

#endif /* SFIBERSSTACKCAL_H */
