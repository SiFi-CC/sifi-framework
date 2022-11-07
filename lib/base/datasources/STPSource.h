// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef STPSOURCE_H
#define STPSOURCE_H

#include "sifi_export.h"

#include "SDataSource.h"

#include <RtypesCore.h> // for Double_t, Int_t

#include <cstdint> // for uint16_t
#include <cstdio>  // for printf
#include <fstream>
#include <memory> // for shared_ptr
#include <string>

#include "TFile.h"
#include "TTree.h"

struct TPHit
{
    Long64_t time;
    Float_t energy;
    UInt_t channelID;

    void print() const
    {
        printf("TOFPET: time = %lld, energy = %f, channelID = %i\n", time, energy, channelID);
    }
};

/**
 * Extends SDataSource to read data from the TOFPET setup.
 */
class SIFI_EXPORT STPSource : public SDataSource
{
public:
    explicit STPSource(uint16_t subevent);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string& filename, size_t length = 0);

private:
    uint16_t subevent;     ///< subevent id
    std::string input;     ///< source file name
    TFile * input_file;    ///< data input file
    TTree *t;
    Long64_t nentries;
    std::shared_ptr<TPHit> hit_cache;
    Long64_t entries_counter;
    enum State
    {
        READING,
        DONE
    } state;
};
#endif /* STPSOURCE_H */
