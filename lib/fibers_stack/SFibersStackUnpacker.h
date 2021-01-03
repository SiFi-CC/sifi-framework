// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKUNPACKER_H
#define SFIBERSSTACKUNPACKER_H

#include "STask.h"

class SCategory;

class SFibersStackUnpacker : public STask
{
protected:
    // members
    SCategory * catFibersRaw;           ///< fibers raw category

public:
    // constructor
    SFibersStackUnpacker();
    // destructor
    virtual ~SFibersStackUnpacker() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSSTACKUNPACKER_H */
