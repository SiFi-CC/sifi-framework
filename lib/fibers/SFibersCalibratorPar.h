// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSCALIBRATORPAR_H
#define SFIBERSCALIBRATORPAR_H

#include "SCalContainer.h"
#include "SFibersLookup.h"

struct SLookupChannel;

class SFibersCalibratorPar : public SCalContainer<6>
{
public:
    using SCalContainer::SCalContainer;

    SLookupChannel* createChannel() const override { return new SFibersChannel; }
};
#endif /* SFIBERSCALIBRATORPAR_H */
