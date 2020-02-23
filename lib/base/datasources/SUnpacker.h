// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SUNPACKER_H
#define SUNPACKER_H

#include <cstddef>
#include <string>
#include <map>

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

class SUnpacker
{
public:
    SUnpacker(uint16_t address);
    // methods
    /// Initialize task
    /// \return success
    virtual bool init() = 0;
    /// Called when reinitializatoin is required
    /// \return success
    virtual bool reinit() { return true; }
    /// Execute task
    /// \return success
    virtual bool execute(unsigned long event, unsigned long seq_number,
        void * buffer, size_t length) = 0;
    /// Finalize task
    /// \return success
    virtual bool finalize() = 0;

    uint16_t getAddress() const { return addr; }

protected:
    uint16_t addr;
};

#endif /* SUNPACKER_H */
