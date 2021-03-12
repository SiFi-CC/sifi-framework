// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSUNPACKER_H
#define SFIBERSUNPACKER_H

#include "STask.h"

class SCategory;

class SFibersUnpacker : public STask
{
protected:
    // members
    SCategory* catFibersRaw; ///< fibers raw category

public:
    // constructor
    SFibersUnpacker();
    // destructor
    virtual ~SFibersUnpacker() = default;

    // methods
    bool init() override;
    bool execute() override;
    bool finalize() override;
};

#endif /* SFIBERSUNPACKER_H */
