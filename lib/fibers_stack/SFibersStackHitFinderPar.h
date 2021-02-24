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
    Float_t fAlpha{0.0};

    Int_t numOfCalibPars = 2;
    Int_t numOfResPars = 3;
    TArrayF fCalibPos;
    TArrayF fCalibPosErr;
    TArrayF fCalibEne;
    TArrayF fCalibEneErr;
    TArrayF fResPos;
    TArrayF fResEne;

  public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    Float_t getA0() const { return fA0; }
    Float_t getLambda() const { return fLambda; }
    Float_t getAlpha() const { return fAlpha; }

    void setA0(Float_t v) { fA0 = v; }
    void setLambda(Float_t v) { fLambda = v; }
    void setAlpha(Float_t v) { fAlpha = v; }

    void setCalibPos(TArrayF f) { fCalibPos = f; }
    void setCalibPosErr(TArrayF f) { fCalibPosErr = f; }
    void setResPos(TArrayF f) { fResPos = f; }
    void setCalibEne(TArrayF f) { fCalibEne = f; }
    void setCalibEneErr(TArrayF f) { fCalibEneErr = f; }
    void setResEne(TArrayF f) { fResEne = f; }
};

#endif /* SFIBERSSTACKHITFINDERPAR_H */
