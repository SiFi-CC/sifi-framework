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

#include "SPar.h"

#include <Rtypes.h>
#include <TArrayF.h>

class SFibersStackDDUnpackerPar : public SPar
{
  private:
    // members
    TArrayF fThreshold; ///< Threshold in ADC (LE) or % (CF)
    Int_t nPolarity;    ///< 0 - neg, 1 - pos
    Int_t nAnaMode;     ///< 0 - LE, 1 - CF
    Int_t nIntMode;     ///< <=0 - TOT, >0 - Limit
    Int_t nDeadTime;    ///< fDeadTime

  public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Return Threshold
    /// \param chan channel number
    /// \return threshold
    Float_t getThreshold(Int_t chan) const;
    /// Return Polarity
    /// \return polarity
    Int_t getPolarity() const { return nPolarity; }
    /// Return Polarity
    /// \return polarity
    Int_t getAnaMode() const { return nAnaMode; }
    /// Return Polarity
    /// \return polarity
    Int_t getIntMode() const { return nIntMode; }
    /// Return DeadTime
    /// \return deadtime
    Int_t getDeadTime() const { return nDeadTime; }
};

#endif /* SFIBERSSTACKDDUNPACKERPAR_H */
