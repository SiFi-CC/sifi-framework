// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSRAWCLUSTERFINDER_H
#define SFIBERSRAWCLUSTERFINDER_H

#include "sifi_export.h"

#include "SDataSource.h"
#include "SUnpacker.h"
#include "STP4to1Source.h"
#include "SCategory.h"
#include "SSiPMCluster.h"
#include "SMultiFibersLookup.h"
#include "SCalContainer.h"
// #include "SFibersRaw.h" //should not be needed in final version
#include "SFibersRawCluster.h"
#include <RtypesCore.h> // for Double_t, Int_t

#include <cstdint> // for uint16_t
#include <cstdio>  // for printf
#include <fstream>
#include <memory> // for shared_ptr
#include <string>

#include "STask.h"

class SSiPMsLookupTable;
class SLocator;

struct fibAddressC
{
    int    mod=-100;
    UInt_t lay=-100;
    UInt_t fi=-100;
    char   side;
    
    void print()
    {
        printf("fibAddressC: mod = %i, lay = %i, fib = %i, side = %c \n", mod, lay, fi, side);
    };
};

struct identifiedFiberDataC
{
    int mod=-100;
    UInt_t lay=-100;
    UInt_t fi=-100;
    char   side;
    Long64_t timeL=-100;    //for TOFPET time must be Long64_t!
    float energyL=-100;
    Long64_t timeR=-100;
    float energyR=-100;
    int topClusID=-100;
    int botClusID=-100;
    //add event or subevent ID?
    
    void print()
    {
        printf("identifiedFiberData: address: %i, %i, %i, left: %lld, %f, right: %lld, %f \n", mod, lay, fi, timeL, energyL, timeR, energyR);
    }
};

enum fiberClusterHitTypeC
{
    uniqueClusterUniqueFiberC = 1, 
    uniqueClusterMultipleFiberC = 2, 
    topUniqueClusterC = 3, //unique only on the top side
    bottomUniqueClusterC = 4, // unique only on the bottom side
    ambiguousClusterC = 5
};

/**
 * \class SFibersRawClusterFinder
 * This class identifies the fiberClusterID given the SiPMID and QDC (4 to 1 coupling)
 */
class SIFI_EXPORT SFibersRawClusterFinder : public STask
{
public:
    explicit SFibersRawClusterFinder();
    
    bool init() override;
    bool execute() override;
    bool finalize() override;
    
private:

    const int n_fibers_per_SiPM = 4;
    std::vector <identifiedFiberDataC> getFibersFromCluster(SSiPMCluster *cluster);
    std::vector <identifiedFiberDataC> CommonFibers(SSiPMCluster* TopCluster, SSiPMCluster* BottomCluster);
    float correctNLC(float qdc);
    float alignQDC(identifiedFiberDataC address, float qdc);
    
    
protected:
    SCategory* catSiPMsHit{nullptr};      ///< category containing SiPMs hits
    SCategory* catSiPMsCluster{nullptr};  ///< category containing SiPMs clusters
    SCategory* catFibersRaw{nullptr};     ///< category containing raw fibers data
    SCategory* catFibersRawClus{nullptr};     ///< category containing raw fiber clusters data
    SMultiFibersLookupTable* fibLookup{nullptr};
    SCalContainer<6>* pTOFPETCalPar;
    SCalContainer<6>* pSiPMTOFPETCalPar;
};
#endif /* SFIBERSRAWCLUSTERFINDER_H */

