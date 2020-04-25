// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCALIBRATOR_H
#define SFIBERSSTACKCALIBRATOR_H

#include "STask.h"

class SCategory;
class SCalContainer;
class SFibersStackCalibratorPar;

class SFibersStackCalibrator : public STask
{
protected:
    // members
    SCategory * catFibersRaw;                   ///< fibers raw category
    SCategory * catFibersCal;                   ///< fibers cal category
    SCalContainer * pCalibratorPar; ///< calibrator parameters

public:
    // constructor
    SFibersStackCalibrator();
    // destructor
    virtual ~SFibersStackCalibrator();

    // methods
    bool init();
    bool execute();
    bool finalize();
};

#endif /* SFIBERSSTACKCALIBRATOR_H */
