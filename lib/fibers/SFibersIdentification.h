// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSIDENTIFICATION_H
#define SFIBERSIDENTIFICATION_H

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

#include "STask.h"

class SSiPMsLookupTable;
class SLocator;

struct fibAddress
{
    int    mod=-100;
    UInt_t lay=-100;
    UInt_t fi=-100;
    char   side;
};

struct identifiedFiberData
{
    UInt_t mod=-100;
    UInt_t lay=-100;
    UInt_t fi=-100;
    Long64_t timeL=-100;
//     Float_t timeL=-100;
    Float_t energyL=-100;
    Long64_t timeR=-100;
//     Float_t timeR=-100;
    Float_t energyR=-100;
    //add event or subevent ID?
    
    void print()
    {
        printf("identifiedFiberData: address: %i, %i, %i, left: %lld, %f, right: %lld, %f \n", mod, lay, fi, timeL, energyL, timeR, energyR);
    }
};

/**
 * \class SFibersIdentification
 * This class identifies the fiberID given the SiPMID and QDC (4 to 1 coupling)
 */
class SIFI_EXPORT SFibersIdentification : public STask
{
public:
    explicit SFibersIdentification();
    
    bool init() override;
    bool execute() override;
    bool finalize() override;
    
//     std::vector<std::shared_ptr<fibAddress>> get4to1FiberFromSiPM(UInt_t SiPMID);
//     UInt_t get4to1SiPMFromFiber(std::vector<std::shared_ptr<fibAddress>> & fiber);
    std::vector<std::shared_ptr<identifiedFiberData>> identifyFibers(std::vector<std::shared_ptr<TP4to1Hit>> & hits);
    
private:
    std::shared_ptr<fibAddress> fibOnlyAddress;
    std::shared_ptr<identifiedFiberData> fibData;
    const int n_fibers_per_SiPM = 4;
};
#endif /* SFIBERSIDENTIFICATION_H */

