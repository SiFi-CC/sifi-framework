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
#include <map>
#include <string>

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

/**
 * Abstract base class for unpacker. The interface contains of following
 * functions:
 *  * init() - init unpacker, called only once,
 *  * reinit() - called when some of the data has changed, e.g. parameters and
 *    lookup tables,
 *  * execute() - executes the task, accept several fields describing the event
 *    and data passed to the unpacker,
 *  * finalize() - makes final steps, e.g. release containers.
 */
class SUnpacker
{
public:
    virtual ~SUnpacker() = default;

    /// Initialize task
    /// \return success
    virtual bool init() { return true; };
    /// Called when reinitializatoin is required
    /// \return success
    virtual bool reinit() { return true; }
    /// Execute task
    /// \param event global event number
    /// \param seq_number sequence number (par file)
    /// \param subevent address of subevent
    /// \param buffer data buffer
    /// \param length size of teh buffer in bytes (uint8_t)
    /// \return success
    virtual bool execute(ulong event, ulong seq_number, uint16_t subevent, void* buffer,
                         size_t length) = 0;
    /// Finalize task
    /// \return success
    virtual bool finalize() { return true; };

    void setSampleTons(float bin_value) { sample_to_ns = bin_value; }

    void setADCTomV(float coeff) { ADC_to_mV = coeff; }

    float getSampleTons(void) { return sample_to_ns; }

    float getADCTomV(void) { return ADC_to_mV; }

private:
    float sample_to_ns;
    float ADC_to_mV;
};

#endif /* SUNPACKER_H */
