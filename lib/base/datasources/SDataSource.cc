// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SDataSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <map>

void SDataSource::addUnpacker(SUnpacker * unpacker, uint16_t address)
{
    if (unpackers[unpacker->getAddress()] != nullptr)
    {
        std::cerr << "Unpacker already exists" << std::endl;
        abort();
    }

    printf("Add unpacker: 0x%x\n", address);
//     if (address != 0x0000)
//         special_addr = address;

    unpackers[address] = unpacker;
}

