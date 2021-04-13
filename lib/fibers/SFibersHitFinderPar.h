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
    Float_t fA0{0.0};     ///< position fit parameter y-intercept
    Float_t fLambda{0.0}; ///< position fit parameter slope
    Float_t fAlpha{0.0};  ///< energy fit paramater slope
    TArrayF fResPos;      ///< store position resolution fit parameters
    TArrayF fResEne;      ///< store energy resolution fit parameters

public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Get A0 parameter
    /// \return A0 parameter
    Float_t getA0() const { return fA0; }
    /// Get Lambda parameter
    /// \return Lambda parameter
    Float_t getLambda() const { return fLambda; }
    /// Get Alpha parameter
    /// \return Alpha parameter
    Float_t getAlpha() const { return fAlpha; }

    /// Get position resolution parameter
    /// \param i array index
    /// \return position resolution paramater at index #i parameter
    Float_t getResPos(UInt_t i) const;
    /// Get energy resolution parameter
    /// \param i array index
    /// \return energy resolution paramater at index #i parameter
    Float_t getResEne(UInt_t i) const;

    /// Set A0 parameter
    /// \param v new value
    void setA0(Float_t v) { fA0 = v; }
    /// Set Lambda parameter
    /// \param v new value
    void setLambda(Float_t v) { fLambda = v; }
    /// Set Alpha parameter
    /// \param v new value
    void setAlpha(Float_t v) { fAlpha = v; }

    /// Set position resolution parameter
    /// \param i array index
    /// \param f new value
    void setResPos(UInt_t i, Float_t f);
    /// Set energy resolution parameter
    /// \param i array index
    /// \param f new value
    void setResEne(UInt_t i, Float_t f);
};

#endif /* SFIBERSHITFINDERPAR_H */
