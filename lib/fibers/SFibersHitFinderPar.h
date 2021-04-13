// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSHITFINDERPAR_H
#define SFIBERSHITFINDERPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <RtypesCore.h> // for Float_t, UInt_t
#include <TArrayF.h>

class SParContainer;

class SIFI_EXPORT SFibersHitFinderPar : public SPar
{
private:
    // members
    Float_t fA0{0.0};     // position fit parameter y-intercept
    Float_t fLambda{0.0}; // position fit parameter slope
    Float_t fAlpha{0.0};  // energy fit paramater slope
    TArrayF fResPos;      // store position resolution fit parameters
    TArrayF fResEne;      // store energy resolution fit parameters

public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    Float_t getA0() const { return fA0; }
    Float_t getLambda() const { return fLambda; }
    Float_t getAlpha() const { return fAlpha; }

    Float_t getResPos(UInt_t i) const;
    Float_t getResEne(UInt_t i) const;

    void setA0(Float_t v) { fA0 = v; }
    void setLambda(Float_t v) { fLambda = v; }
    void setAlpha(Float_t v) { fAlpha = v; }

    void setResPos(UInt_t i, Float_t f);
    void setResEne(UInt_t i, Float_t f);
};

#endif /* SFIBERSHITFINDERPAR_H */
