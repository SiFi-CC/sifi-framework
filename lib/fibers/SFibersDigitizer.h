// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSDIGITIZER_H
#define SFIBERSDIGITIZER_H

#include "STask.h"

#include <RtypesCore.h>

#include <vector>

class SCategory;
class SFibersDigitizerPar;
class SFibersGeomPar;

class SFibersDigitizer : public STask
{
private:
    // members
    SCategory* catGeantFibersRaw; ///< genat raw
    SCategory* catFibersCalSim;   ///< fibers cal

    SFibersDigitizerPar* pDigiPar; ///< digitizer parameters
    SFibersGeomPar* pGeomPar;      ///< geometry parameters

    std::vector<std::vector<Int_t>> layer_fiber_limit; ///< store fibers offset

public:
    // constructor
    SFibersDigitizer();
    // destructor
    virtual ~SFibersDigitizer() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSDIGITIZER_H */
