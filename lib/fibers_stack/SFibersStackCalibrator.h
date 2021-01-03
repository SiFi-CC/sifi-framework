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
template<int N>
class SCalContainer;

class SFibersStackCalibrator : public STask
{
protected:
    // members
    SCategory * catFibersRaw;           ///< fibers raw category
    SCategory * catFibersCal;           ///< fibers cal category
    SCalContainer<3> * pCalibratorPar;  ///< calibrator parameters

public:
    // constructor
    SFibersStackCalibrator();
    // destructor
    virtual ~SFibersStackCalibrator() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSSTACKCALIBRATOR_H */
