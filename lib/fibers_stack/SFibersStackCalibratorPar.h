// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCALIBRATORPAR_H
#define SFIBERSSTACKCALIBRATORPAR_H

#include "SCalContainer.h"
#include "SFibersStackLookup.h"

#include <Rtypes.h>

class SFibersStackCalibratorPar : public SCalContainer
{
public:
    SFibersStackCalibratorPar(const std::string & container): SCalContainer(container) {};

    SLookupChannel * createChannel() const override { return new SFibersStackChannel; }
};
#endif /* SFIBERSSTACKCALIBRATORPAR_H */
