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

#include "sifi_export.h"

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
    /// Initialize task
    /// \return success
    virtual bool init() { return true; };
    /// Called when reinitializatoin is required
    /// \return success
    virtual bool reinit() { return true; }
    /// Execute task
    /// \param event global event number
    /// \param seq_number sequence number (par file)
    /// \param address address of subevent
    /// \param buffer data buffer
    /// \param length size of teh buffer in bytes (uint8_t)
    /// \return success
    virtual bool execute(unsigned long event, unsigned long seq_number, uint16_t address,
        void * buffer, size_t length) = 0;
    /// Finalize task
    /// \return success
    virtual bool finalize() { return true; };
};

#endif /* SUNPACKER_H */
