// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKHITFINDERPAR_H
#define SFIBERSSTACKHITFINDERPAR_H

#include "SPar.h"
//#include "SParContainer.h"

#include <Rtypes.h>
#include <TArrayF.h>
#include <TArrayI.h>

class SFibersStackHitFinderPar : public SPar
{
  private:
    // members
    Float_t fA0;
    Float_t fLambda;

  public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;
    

    Float_t getfA0() const { return fA0; }
    Float_t getfLambda() const { return fLambda; }
};

#endif /* SFIBERSSTACKHITFINDERPAR_H */

