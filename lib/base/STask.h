// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef STASK_H
#define STASK_H

class STask
{
public:
    // methods
    /// Initialize task
    /// \return success
    virtual bool init() = 0;
    /// Called when reinitializatoin is required
    /// \return success
    virtual bool reinit() { return true; }
    /// Execute task
    /// \return success
    virtual bool execute() = 0;
    /// Finalize task
    /// \return success
    virtual bool finalize() = 0;
};

#endif /* STASK_H */
