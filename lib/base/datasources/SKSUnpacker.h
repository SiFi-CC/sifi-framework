
// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SKSUNPACKER_H
#define SKSUNPACKER_H

#include "sifi_export.h"

#include "SUnpacker.h"

#include <cstddef>
#include <string>
#include <fstream>

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

/**
 * Unpacker for Desktop Digitizer data. It is a abstract base class which must
 * provide implementation to the data, mainly lookup table.
 */
class SIFI_EXPORT SKSUnpacker : public SUnpacker
{
public:
    SKSUnpacker();

    // methods
    /// Initialize task
    /// \return success
    virtual bool init() override;
    /// Called when reinitializatoin is required
    /// \return success
    virtual bool reinit() override;
    /// Execute task
    /// \param event event number
    /// \param seq_number sequence number
    /// \param subevent subevent id
    /// \param buffer data buffer
    /// \param length buffer length
    /// \return success
    virtual bool execute(ulong event, ulong seq_number, uint16_t subevent,
                         void * buffer, size_t length) override;

    /// Set expected data len, we speak about floats not bytes.
    /// \param length size of the array in terms of floats
    void setDataLen(size_t length) { data_length = length;}

    static void saveSamples(bool save) { save_samples = save; }

protected:
    /// Decode buffer. This function must be implemented in deriving class to
    /// provide interpretation for data.
    /// \param subevent subevent id
    /// \param data data buffer
    /// \param length buffer length
    /// \return success
    virtual bool decode(uint16_t subevent, float * data, size_t length) = 0;

protected:
    static bool save_samples;

private:
    size_t data_length;         ///< data buffer length
};

#endif /* SKSUNPACKER_H */
