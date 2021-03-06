// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSCLUSTERFINDERPAR_H
#define SFIBERSCLUSTERFINDERPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <RtypesCore.h> // for Int_t

class SParContainer;

class SIFI_EXPORT SFibersClusterFinderPar : public SPar
{
private:
    // members
    Int_t fClusterMode{0}; ///< cluster mode: 0 - AC; 1 - HF; 2 - FF

public:
    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Get cluste mode
    /// \return cluster mode
    Int_t getClusterMode() const { return fClusterMode; }

    /// Set cluster mode
    /// \param m new mode
    void setClusterMode(Int_t m) { fClusterMode = m; }
};

#endif /* SFIBERSCLUSTERFINDERPAR_H */
