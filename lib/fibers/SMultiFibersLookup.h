// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SMULTIFIBERSLOOKUP_H
#define SMULTIFIBERSLOOKUP_H


#include "sifi_export.h"

#include "SLookup.h"

#include <cstdio>
#include <memory> // for make_unique, unique_ptr
#include <stdint.h>
#include <sys/types.h> // for uint
/**
 * 
 * 
 * 
 */
struct SIFI_EXPORT SMultiFibersChannel : public SLookupChannel
{
    /// handles the association of max four fibers to a SiPM
    std::vector<std::vector<std::string> > vecFiberAssociations;
    char side; ///< side of a fiber, either 'l' or 'r'

    SMultiFibersChannel() {}
    virtual ~SMultiFibersChannel() {}

    virtual uint read(const char* buffer) override;
    virtual uint write(char* buffer, size_t n) const override;
    virtual void print(bool newline = true, const char* prefix = "") const override;
    virtual uint64_t quickHash() const override;
    virtual void fromHash(uint64_t hash) override;
};

class SIFI_EXPORT SMultiFibersLookupTable : public SLookupTable
{
public:
    using SLookupTable::SLookupTable;
    virtual ~SMultiFibersLookupTable() = default;

    // methods
    void tokenize(std::string str, std::vector<std::string> &token_v, char delimiter);
    virtual std::unique_ptr<SLookupChannel> createChannel() const override
    {
        return std::make_unique<SMultiFibersChannel>();
    }
};

#endif /* SMULTIFIBERSLOOKUP_H */
