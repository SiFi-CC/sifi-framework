// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef MDDUNPACKER_H
#define MDDUNPACKER_H

#include "SUnpacker.h"

#include <cstddef>
#include <string>
#include <fstream>

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

class SDDUnpacker : public SUnpacker
{
public:
    SDDUnpacker(uint16_t address);
    virtual ~SDDUnpacker();

    // methods
    /// Initialize task
    /// \return success
    virtual bool init();
    /// Called when reinitializatoin is required
    /// \return success
    virtual bool reinit();
    /// Execute task
    /// \return success
    virtual bool execute(unsigned long event, unsigned long seq_number,
        void * buffer, size_t length);
    /// Finalize task
    /// \return success
    virtual bool finalize() { return true; }

    void setDataLen(size_t length) { data_length = length;}

protected:
    virtual bool decode(float * data, size_t length) = 0;

private:
    size_t data_length;
};

#endif /* SDDUNPACKER_H */
