// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCLUSTERFINDERPAR_H
#define SFIBERSSTACKCLUSTERFINDERPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <Rtypes.h>
#include <TArrayF.h>
#include <TArrayI.h>

class SIFI_EXPORT SFibersStackClusterFinderPar : public SPar
{
  private:
    // members
    Int_t fClusterMode{0};    // 0 - AC; 1 - HF; 2 - FF

  public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    Int_t getClusterMode() const { return fClusterMode; }

    void setClusterMode(Int_t m) { fClusterMode = m; }
};

#endif /* SFIBERSSTACKCLUSTERFINDERPAR_H */
