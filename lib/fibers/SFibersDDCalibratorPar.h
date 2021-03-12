// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSDDCALIBRATORPAR_H
#define SFIBERSDDCALIBRATORPAR_H

#include "SCalContainer.h"
#include "SFibersLookup.h"

#include <Rtypes.h>

class SFibersDDCalibratorPar : public SCalContainer<2>
{
public:
    using SCalContainer::SCalContainer;

    SLookupChannel* createChannel() const override { return new SFibersChannel; }
};
#endif /* SFIBERSDDCALIBRATORPAR_H */
