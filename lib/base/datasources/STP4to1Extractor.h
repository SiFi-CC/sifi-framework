// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef STP4TO1EXTRACTOR_H
#define STP4TO1EXTRACTOR_H

#include "sifi_export.h"

#include "TTree.h"
#include "SDataSource.h"
#include "SUnpacker.h"
#include "STP4to1Source.h"
#include <RtypesCore.h> // for Double_t, Int_t

#include <cstdint> // for uint16_t
#include <cstdio>  // for printf
#include <fstream>
#include <memory> // for shared_ptr
#include <string>

#include "TFile.h"
#include "TTree.h"

class SUnpacker;
class SCategory;
class SSiPMsLookupTable;
class SFibersIdentification;
struct SiPMHit
{
    Long64_t time=-100;
    Float_t energy=-100;
    Int_t channelID=0;

    void print() const
    {
        printf("TOFPET 4to1: time = %lld, energy = %f, swSiPMID = %i\n", time, energy, channelID);
    }
};

/**
 * \class STP4to1Extractor
 * This class extracts data from STP4to1Source, fills to SiPMHit category, calls FiberIdentification and passes the identified fiber data to STP4to1Unpacker
 */
class SIFI_EXPORT STP4to1Extractor
{
public:
    explicit STP4to1Extractor(uint16_t subevent, std::map<uint16_t, SUnpacker*> unpackers);
    virtual ~STP4to1Extractor();
        bool extract_open();
        bool extract_close();
        bool extract_readCurrentEvent(std::vector<std::shared_ptr<TP4to1Hit>> & hits);
        bool write_to_tree(std::vector<std::shared_ptr<TP4to1Hit>> & hits);

private:
    uint16_t subevent;     ///< subevent id
    std::map<uint16_t, SUnpacker*> unpackers;
};
#endif /* STP4TO1EXTRACTOR_H */
