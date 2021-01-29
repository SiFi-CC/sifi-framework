// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include <TClass.h>
#include <TClonesArray.h>

#include "SKSUnpacker.h"

bool SKSUnpacker::save_samples  = false;

SKSUnpacker::SKSUnpacker() : data_length(0)
{
}

bool SKSUnpacker::init()
{
    if (!data_length)
    {
        std::cerr << "Data length not configured." << std::endl;
        abort();
    }

    return true;
}

bool SKSUnpacker::reinit()
{
    return init();
}

bool SKSUnpacker::execute(unsigned long /*event*/, unsigned long /*seq_number*/,
                          uint16_t address, void * buffer, size_t length)
{
    if (length != data_length * sizeof(float))
     {
        std::cerr << "Buffer length " << data_length
                  << " does not fit source length " << length
                  << "." << std::endl;
        abort();
    }

//     bool flag = false;
//     printf("ifstream = 0x%x\n", istream);
//     char [c];
//     for (uint ii = 0; ii < sample_length; ++ii) {
//         (*istream).read((char*)&sample_buffer, sizeof(sample_buffer[0])*sample_length);
//         flag = (*istream).eof();
//         if (flag)
//             return false;
//     }

    return decode(address, (float *)buffer, data_length);
}

