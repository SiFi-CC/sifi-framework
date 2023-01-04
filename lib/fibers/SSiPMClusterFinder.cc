// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SSiPMClusterFinder.h"
#include "SSiPMCluster.h"
#include "SCategory.h"
#include "SSiPMHit.h"
#include "SLocator.h"
#include "SPar.h"
#include "SiFi.h"

#include <RtypesCore.h>
#include <TObject.h>
#include <TVector3.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory> 
#include <utility>
#include <vector>

/**
 * \class SSiPMClusterFinder
 * \ingroup lib_fibers
 *
 * A task class, which finds clusters of SiPM hits. A cluster consists of SiPMs which are direct
 * neighbours (touching sides or corners).
 * 
 **/

bool SSiPMClusterFinder::init()
{
    catSiPMsHit = sifi()->buildCategory(SCategory::CatSiPMHit);
    
    if(!catSiPMsHit)
    {
        std::cerr << "No CatSiPMHit category!" << std::endl;
        return false;
    }
    
   catSiPMsCluster = sifi()->buildCategory(SCategory::CatSiPMClus);
   
   if(!catSiPMsCluster)
   {
       std::cerr << "No CatSiPMClus category!" << std::endl;
   }
    
    return true;
}

bool checkIfNeighbours(SSiPMHit* hit_1, SSiPMHit* hit_2)
{
 
    /// TODO checking if two given sipms are neighbours
    
}

bool SSiPMClusterFinder::execute()
{
    
    int size = catSiPMsHit->getEntries();
    for (int i = 0; i < size; ++i)
    {
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(i));
        if (!pHit)
        {
            printf("SiPMHit doesn't exists!\n");
            continue;
        }
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t element = 0;
        char side = '\0';
        pHit->getAddress(mod, lay, element, side);

        Float_t qdc = pHit->getQDC();
        Long64_t time = pHit->getTime();
        // checkIfNeighbours() somehow?
        // then construct SSiPMCluster and addHit() and point(0, 0, 0)
    }    
    return true;
}

bool SSiPMClusterFinder::finalize()
{
    return true;
}
