// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCALIBRATORPAR_H
#define SFIBERSSTACKCALIBRATORPAR_H

#include "SPar.h"

class SFibersStackCalibratorPar : public SPar
{
private:
    // members
    Float_t fAdcGain;           ///< ADC gain
    Float_t fAdcOffset;         ///< ADC offset (bias)

public:
    bool getParams(SParContainer * parcont);
    bool putParams(SParContainer * parcont) const;
    void clear();
    void print() const;

    /// Return ADC gain
    /// \return gain
    Int_t getAdcGain() const { return fAdcGain; }
    /// Return ADC offset
    /// \return offset
    Int_t getAdcOffset() const { return fAdcOffset; }
};

#endif /* SFIBERSSTACKCALIBRATORPAR_H */
