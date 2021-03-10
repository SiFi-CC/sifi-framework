// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKDIGITIZERPAR_H
#define SFIBERSSTACKDIGITIZERPAR_H

#include "SPar.h"

class SFibersStackDigitizerPar : public SPar
{
  protected:
    // members

  public:
    // methods
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
};

#endif /* SFIBERSSTACKDIGITIZERPAR_H */
