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

/**
 * Add an unpacker fo source
 *
 * \param unpacker the unpacker object
 * \param address addresses which the unpacker must handle
 */
void SDataSource::addUnpacker(SUnpacker* unpacker, const std::vector<uint16_t>& address)
{
    for (auto addr : address)
    {
        printf("Add unpacker: 0x%x\n", addr);
        if (unpackers[addr] != nullptr)
        {
            std::cerr << "Unpacker already exists" << std::endl;
            abort();
        }
        unpackers[addr] = unpacker;
    }
}

SDataSource::~SDataSource()
{
    for (auto& unp : unpackers)
        delete unp.second;
}
