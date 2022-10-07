// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCBSOURCE_H
#define SCBSOURCE_H

#include "sifi_export.h"

#include "SDataSource.h"

#include <RtypesCore.h> // for Double_t, Int_t

#include <cstdint> // for uint16_t
#include <cstdio>  // for printf
#include <fstream>
#include <memory> // for shared_ptr
#include <string>

struct CBHit
{
    Int_t trgID = -100;
    Double_t tstamp_us = -100;
    Int_t brd = -100; 
    Int_t ch = -100;
    Double_t q_lg = -100;
    std::string q_hg = "";

    void print() const
    {
        printf("Citiroc: trgID = %i, tstamp_us = %f, brd = %i, ch = %i, q_lg = %f,q_hg = %s\n", trgID, tstamp_us, brd, ch, q_lg, q_hg.c_str());
    }
};

/**
 * Extends SDataSource to read data from the Citiroc setup.
 */
class SIFI_EXPORT SCBSource : public SDataSource
{
public:
    explicit SCBSource(uint16_t subevent);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string& filename, size_t length = 0);

private:
    uint16_t subevent;     ///< subevent id
    std::string input;     ///< source file name
    std::ifstream istream; ///< input file stream
    enum State
    {
        INIT,
        READING,
        DONE
    } state;
    std::shared_ptr<CBHit> hit_cache;
};
#endif /* SCBSOURCE_H */
