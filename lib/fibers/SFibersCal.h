// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSCAL_H
#define SFIBERSCAL_H

#include "sifi_export.h"

#include "SFibersRaw.h"

#include <Rtypes.h>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersCal : public SFibersRaw
{
public:
    // constructors
    SFibersCal() = default;
    virtual ~SFibersCal() = default;

    ClassDef(SFibersCal, 1); // container for fibers stack calibrated data
};

#endif /* SFIBERSCAL_H */
