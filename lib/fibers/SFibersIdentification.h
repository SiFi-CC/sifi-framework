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

#include "SDataSource.h"
#include "SUnpacker.h"
#include "STP4to1Source.h"
#include "SCategory.h"
#include "SSiPMCluster.h"
#include "SMultiFibersLookup.h"
#include "SCalContainer.h"
#include "SFibersRaw.h"
#include <RtypesCore.h> // for Double_t, Int_t

#include <cstdint> // for uint16_t
#include <cstdio>  // for printf
#include <fstream>
#include <memory> // for shared_ptr
#include <string>

#include "STask.h"

class SSiPMsLookupTable;
class SLocator;

struct fibAddress
{
    int    mod=-100;
    UInt_t lay=-100;
    UInt_t fi=-100;
    char   side;
    
    void print()
    {
        printf("fibAddress: mod = %i, lay = %i, fib = %i, side = %c \n", mod, lay, fi, side);
    };
};

struct identifiedFiberData
{
    UInt_t mod=-100;
    UInt_t lay=-100;
    UInt_t fi=-100;
    Long64_t timeL=-100;    //for TOFPET time must be Long64_t!
    Float_t energyL=-100;
    Long64_t timeR=-100;
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
    
private:

    const int n_fibers_per_SiPM = 4;
    std::vector <fibAddress> getFibersFromCluster(SSiPMCluster *cluster);
    std::vector <fibAddress> CommonFibers(SSiPMCluster* TopCluster, SSiPMCluster* BottomCluster);
    float correctNLC(float qdc);
    float alignQDC(fibAddress address, float qdc);
    
    
protected:
    SCategory* catSiPMsHit{nullptr};      ///< category containing SiPMs hits
    SCategory* catSiPMsCluster{nullptr};  ///< category containing SiPMs clusters
    SCategory* catFibersRaw{nullptr};     ///< category containing raw fibers data
    SMultiFibersLookupTable* fibLookup{nullptr};
    SCalContainer<6>* pTOFPETCalPar;
};
#endif /* SFIBERSIDENTIFICATION_H */

