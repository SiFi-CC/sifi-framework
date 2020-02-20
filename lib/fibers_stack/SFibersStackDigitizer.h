// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKDIGITIZER_H
#define SFIBERSSTACKDIGITIZER_H

#include "STask.h"

#include <vector>

class SCategory;
class SFibersStackDigitizerPar;
class SFibersStackGeomPar;

class SFibersStackDigitizer : public STask
{
private:
    // members
    SCategory * catGeantFibersRaw;          ///< genat raw
    SCategory * catFibersCalSim;            ///< fibers cal

    SFibersStackDigitizerPar * pDigiPar;    ///< digitizer parameters
    SFibersStackGeomPar * pGeomPar;         ///< geometry parameters

    std::vector<std::vector<Int_t> > layer_fiber_limit; ///< store fibers offset

public:
    // constructor
    SFibersStackDigitizer();
    // destructor
    virtual ~SFibersStackDigitizer();

    // methods
    bool init();
    bool execute();
    bool finalize();
};

#endif /* SFIBERSSTACKDIGITIZER_H */
