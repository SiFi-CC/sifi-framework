// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKDDUNPACKERPAR_H
#define SFIBERSSTACKDDUNPACKERPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <Rtypes.h>
#include <TArrayF.h>
#include <TArrayI.h>

class SIFI_EXPORT SFibersStackDDUnpackerPar : public SPar
{
  private:
    // members
    TArrayF fThreshold;     ///< Threshold in ADC (LE) or % (CF)
    TArrayF fVetoThreshold; ///< Threshold for determination of fVeto flag (in ADC)
    TArrayI fBLMode;        ///< 0 - dynamic BL subtraction, >0 - constant BL, given value subtracted
    Int_t nPolarity;    ///< 0 - neg, 1 - pos
    Int_t nAnaMode;     ///< 0 - LE, 1 - CF
    Int_t nIntMode;     ///< 0 - TOT, >0 - Limit
    Int_t nDeadTime;    ///< fDeadTime

  public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Returns threshold
    /// \param chan channel number
    /// \return threshold
    Float_t getThreshold(Int_t chan) const;
    /// Returns threshold for determination of fVeto flag
    /// \param chan channel number
    /// \return veto threshold
    Float_t getVetoThreshold(Int_t chan) const;
    /// Returns polarity
    /// \return polarity
    Int_t getPolarity() const { return nPolarity; }
    /// Returns analysis mode
    /// \return analysis mode
    Int_t getAnaMode() const { return nAnaMode; }
    /// Returns integration mode
    /// \return integration mode
    Int_t getIntMode() const { return nIntMode; }
    /// Returns dead time
    /// \return dead time
    Int_t getDeadTime() const { return nDeadTime; }
    /// Returns baseline subtraction mode
    /// \param chan channel
    /// \return base line subtraction mode 
    Int_t getBLMode(Int_t chan) const;
};

#endif /* SFIBERSSTACKDDUNPACKERPAR_H */
