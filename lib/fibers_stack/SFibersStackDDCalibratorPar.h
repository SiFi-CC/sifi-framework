// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKDDCALIBRATORPAR_H
#define SFIBERSSTACKDDCALIBRATORPAR_H

#include "SCalContainer.h"
#include "SFibersStackLookup.h"

#include <Rtypes.h>

class SFibersStackDDCalibratorPar : public SCalContainer<2>
{
public:
    using SCalContainer::SCalContainer;

    SLookupChannel* createChannel() const override { return new SFibersStackChannel; }
};
#endif /* SFIBERSSTACKDDCALIBRATORPAR_H */
