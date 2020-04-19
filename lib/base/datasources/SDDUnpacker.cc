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

#include "SDDUnpacker.h"

SDDUnpacker::SDDUnpacker(uint16_t address)
    : SUnpacker(address)
    , data_length(0)
{
}

SDDUnpacker::~SDDUnpacker()
{
}


bool SDDUnpacker::init()
{
    if (!data_length)
    {
        std::cerr << "Data length not configured." << std::endl;
        abort();
    }

    return true;
}

bool SDDUnpacker::reinit()
{
    return init();
}

bool SDDUnpacker::execute(unsigned long event, unsigned long seq_number,
                          void * buffer, size_t length)
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

    return decode((float *)buffer, data_length);
    return true;
}
