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
/////
        std::vector<std::vector<UInt_t>> SiPMadresses;
        std::vector<UInt_t> ja;
/////
    
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

/////
        ja.push_back(i);
        ja.push_back(mod);
        ja.push_back(lay);
        ja.push_back(element);


        if(side=='r'){
            ja.push_back(0);

        }
        if(side=='l'){
            ja.push_back(1);

        }
        SiPMadresses.push_back(ja);
        ja.clear();
/////
    }    
/////

 // algorithm for clustering- ugly version          
           std::vector<std::vector<UInt_t>> cluster;
           std::vector<UInt_t> cl;
           int not_cluster=1;
           std::vector<std::vector<std::vector<UInt_t>>> clusters;
           std::vector<std::vector<UInt_t>> candidates;
           int cluster_size;
           for(int i=0; i<SiPMadresses.size();i++)
                {
                    candidates.push_back(SiPMadresses[i]);
                }
            while(not_cluster){
                not_cluster=0;
                cluster.clear();
                if(!SiPMadresses.empty()){
                    cl.push_back(SiPMadresses[0][0]);
                    cl.push_back(SiPMadresses[0][1]);
                    cl.push_back(SiPMadresses[0][2]);
                    cl.push_back(SiPMadresses[0][3]);
                    cl.push_back(SiPMadresses[0][4]);
                    cluster.push_back(cl);
                    SiPMadresses.erase(SiPMadresses.begin());
                    candidates.erase(candidates.begin());
                
                for(int i=0; i<SiPMadresses.size();i++)
                {
                    cluster_size=cluster.size();
                    for(std::vector<UInt_t> j : cluster)
                    {
                        cl.clear();
                        if (j[1]==SiPMadresses[i][1] and j[4]==SiPMadresses[i][4] and abs((int)(j[2]-SiPMadresses[i][2]) )<=1 and abs((int)(j[3]-SiPMadresses[i][3]) )<=1)
                        {
                            cl.push_back(SiPMadresses[i][0]);
                            cl.push_back(SiPMadresses[i][1]);
                            cl.push_back(SiPMadresses[i][2]);
                            cl.push_back(SiPMadresses[i][3]);
                            cl.push_back(SiPMadresses[i][4]);
                            cluster.push_back(cl);
                            candidates.erase(candidates.begin()+i);
                            break;
                        }
                    }
                    if(cluster_size==cluster.size())
                        {
                            not_cluster=not_cluster+1;
                        }
                    }    
                }
                clusters.push_back(cluster); 
                SiPMadresses=candidates;
            }
// clusters_final is vector of vector of event's idx 
        std::vector<std::vector<Int_t>> clusters_final;
        std::vector<Int_t> cl_f;
        for(int i=0; i<clusters.size(); i++)
        {
            cl_f.clear();
            for(int j=0;j<clusters[i].size();j++){
                cl_f.push_back(clusters[i][j][0]);
            }
        clusters_final.push_back(cl_f);
        }
        
//    
//    //change the code below by inserting the fiber identification algorithm and based on the algorithm, fill the allFibData structure for all subevents
//    int n_subevents = 5; //number of subevents
//    for (int i = 0; i < n_subevents; i++)
//    {
//        fibData->energyL=0.0;
//        fibData->timeL=0.0;
//        fibData->energyR=0.0;
//        fibData->timeR=0.0;
//        fibData->mod=0;
//        fibData->lay=0;
//        fibData->fi=0;
//        allFibData.push_back(fibData);
//    }
             SiPMadresses.clear();
/////
        for(Int_t i=0; i < clusters_final.size(); ++i) {
            SLocator loc(1);
            loc[0] = i;
            SSiPMCluster* pClus = dynamic_cast<SSiPMCluster*>(catSiPMsCluster->getObject(loc));
            if (!pClus)
            {
                pClus = reinterpret_cast<SSiPMCluster*>(catSiPMsCluster->getSlot(loc));
                pClus = new (pClus) SSiPMCluster;
                pClus->Clear();
            }
            pClus->setAddress(i);
            for(Int_t j=0; j < clusters_final[i].size(); ++j) {
                pClus->addHit(clusters_final[i][j]);
                pClus->getPoint().SetXYZ(0, 0, 0);
                pClus->catSiPMsHit = catSiPMsHit;
            }
            pClus->print();
        }

    return true;
}

bool SSiPMClusterFinder::finalize()
{
    return true;
}
