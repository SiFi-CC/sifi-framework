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

#include "sifi_export.h"

#include "SPar.h"

#include <Rtypes.h>
#include <TArrayF.h>
#include <TArrayI.h>

class SIFI_EXPORT SFibersStackHitFinderPar : public SPar
{
  private:
    // members
    Float_t fA0{0.0};
    Float_t fLambda{0.0};

  public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    Float_t getA0() const { return fA0; }
    Float_t getLambda() const { return fLambda; }

    void setA0(Float_t v) { fA0 = v; }
    void setLambda(Float_t v) { fLambda = v; }
};

#endif /* SFIBERSSTACKHITFINDERPAR_H */
