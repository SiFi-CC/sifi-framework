// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSCALIBRATOR_H
#define SFIBERSCALIBRATOR_H

#include "STask.h"

class SCategory;
template <int N> class SCalContainer;

class SFibersCalibrator : public STask
{
protected:
    // members
    SCategory* catFibersRaw;          ///< fibers raw category
    SCategory* catFibersCal;          ///< fibers cal category
    SCalContainer<6>* pCalibratorPar; ///< calibrator parameters

public:
    // constructor
    SFibersCalibrator();
    // destructor
    virtual ~SFibersCalibrator() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSCALIBRATOR_H */
