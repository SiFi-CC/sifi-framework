// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSDIGITIZERPAR_H
#define SFIBERSDIGITIZERPAR_H

#include "SPar.h"

class SFibersDigitizerPar : public SPar
{
protected:
    // members

public:
    // methods
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
};

#endif /* SFIBERSDIGITIZERPAR_H */
