// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef STP4TO1SOURCE_H
#define STP4TO1SOURCE_H

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

class SCategory;
class STP4to1Extractor;

struct TP4to1Hit
{
    Long64_t time=-100;
    Float_t energy=-100;
    UInt_t channelID=0;

    void print() const
    {
        printf("TOFPET 4to1: time = %lld, energy = %f, hwSiPMID = %i\n", time, energy, channelID);
    }
};

/**
 * Extends SDataSource to read data from the TOFPET setup.
 */
class SIFI_EXPORT STP4to1Source : public SDataSource
{
public:
    explicit STP4to1Source(uint16_t subevent);
    virtual ~STP4to1Source();

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void setInput(const std::string& filename, size_t length = 0);
    void setEntriesOffset(Long64_t i) { entries_offset = i; };

private:
    uint16_t subevent;     ///< subevent id
    std::string input;     ///< source file name
    TFile * input_file;    ///< data input file
    TTree *t;              ///< TOFPET2 DAQ generated singles TTree
    Long64_t nentries;
    std::shared_ptr<TP4to1Hit> hit_cache;
    Long64_t entries_offset, entries_counter;
    SCategory* catSiPMHit;
    STP4to1Extractor * extractor;
    enum State
    {
        INIT,
        READING,
        DONE
    } state;
};
#endif /* STP4TO1SOURCE_H */
