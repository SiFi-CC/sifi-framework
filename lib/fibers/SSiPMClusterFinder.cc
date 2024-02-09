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
// #include "SCalContainer.h"
// #include "SFibersLookup.h"

#include <RtypesCore.h>
#include <TObject.h>
#include <TVector3.h>

#include <algorithm>
#include <cmath>
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
    catSiPMsHit = sifi()->getCategory(SCategory::CatSiPMHit);
    
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
    int mod_1, lay_1, ele_1;
    char side_1;
    
    int mod_2, lay_2, ele_2;
    char side_2;
    
    hit_1->getAddress(mod_1, lay_1, ele_1, side_1);
    hit_2->getAddress(mod_2, lay_2, ele_2, side_2);
    
    if(mod_1 != mod_2) // if two hits were in different modules they are not neighbours
        return false;
    
    if(side_1 != side_2) // if two hits were on different sides they are not neighbours
    {
        return false;
    }
    
    if(abs(lay_1 - lay_2) < 2 &&    // if two hits have neighbouring element and layer numbers they are neighbours
       abs(ele_1 - ele_2) < 2)
    {
        return true;
    }
    else
        return false;
}


bool SSiPMClusterFinder::execute()
{
    int nhits = catSiPMsHit->getEntries(); // number of hits in current event
    bool hit_assigned = false; // flag indicating whether hit was already assigned to cluster
    int nclus = 0;

//     for (int i = 0; i < nhits; ++i) // loop over all hits in current event
//     {
//         SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(i)); // getting a hit 
//         pHit->print();
//     }
        
    std::vector<SSiPMCluster*> clusters; // auxiliary array for created clusters
    
    SLocator loc(1);
    
    for (int i = 0; i < nhits; ++i) // loop over all hits in current event
    {
        hit_assigned = false;
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(i)); // getting a hit 
        
        int sid = 0;
        pHit->getChannel(sid);
        if(sid == 800)
        {
            hit_assigned = true;
            continue;
        }
        
        if(clusters.size() == 0) // if there are no clusters yet, create the first one
        {
            SSiPMCluster *pClus = new SSiPMCluster(); 
            pClus->addHit(pHit->getID()); // adding first hit to the first cluster
            clusters.push_back(pClus);
    
            hit_assigned = true;
        }
        else // if there are some clusters already 
        {
            nclus = clusters.size();
            for(int c = 0; c < nclus; ++c) // iterate over clusters
            {
                if(hit_assigned)
                    break;
                
                std::vector<Int_t> hits = clusters[c]->getHitsArray(); // get list of hits within a cluster
                int nhit_in_clus = hits.size();
                
                for(int h = 0; h < nhit_in_clus; ++h) // iterate over hits
                {
                    if(hit_assigned)
                        break;

                    SSiPMHit* pHit_in_clus = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(hits[h]));

                    if(checkIfNeighbours(pHit, pHit_in_clus)) // check if current hit is a neighbour of any of the hits in the cluster 
                    {
                        clusters[c]->addHit(pHit->getID()); // if yes, then this hit should belong to that cluster
                        hit_assigned = true;
                    }
                }
            }
        }
        if(!hit_assigned)
        {
           SSiPMCluster *pClus = new SSiPMCluster(); 
           pClus->addHit(pHit->getID());
           clusters.push_back(pClus);
    
          hit_assigned = true;
          }
    }
    
    // at this point clusters only contain lists of SiPM hits
    // below other cluster characteristics are determined and assigned
    
     nclus = clusters.size();
    
    for(int c=0; c < nclus; ++c) // iterating over clusters
    {
        std::vector<Int_t> hits = clusters[c]->getHitsArray(); // getting hits in clusters
        int nhit_in_clus = hits.size();
        
        clusters[c]->setID(c); // setting cluster ID
        
        SSiPMHit* pHit_in_clus = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(hits[0]));
        int m, l, e;
        char s;
        pHit_in_clus->getAddress(m, l, e, s);
        clusters[c]->setAddress(m, s); // setting cluster address (location)
        
        Long64_t time = pHit_in_clus->getTime();
        float charge = 0;
        float alignedCharge = 0;
        TVector3 position(0, 0, 0);
        TVector3 errors(0, 0, 0);
        
        for(int h = 0; h < nhit_in_clus; ++h)
        {
            SSiPMHit* pHit_in_clus = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(hits[h]));
            pHit_in_clus->getAddress(m, l, e, s);
            if(pHit_in_clus->getTime() < time) // cluster time is determined as the time of the earliest hit
            {
                time = pHit_in_clus->getTime();
            }
            
            charge += pHit_in_clus->getQDC(); // charge is determined as sum of all hits charges
            alignedCharge += pHit_in_clus->getAlignedQDC(); // aligned charge is determined as sum of all aligned hits charges
            
            position = position + pHit_in_clus->getQDC() * TVector3(e, 0, l); // position calculated with COG
        }
        
        position = 1./charge * position;

        clusters[c]->setTime(time);
        clusters[c]->setQDC(charge);
        clusters[c]->setAlignedQDC(alignedCharge);
        clusters[c]->setPoint(position);
        clusters[c]->setErrors(errors); // at the moment position uncertainty is not calculated so we set it to 0,0,0
        
        // adding clusters to category
        
        loc[0] = c;
        SSiPMCluster *pClus = reinterpret_cast<SSiPMCluster*>(catSiPMsCluster->getSlot(loc));
        pClus = new (pClus) SSiPMCluster(*clusters[c]);
    }
    
//     print();

    return true;
}



// // float alignQDC(SSiPMHit *sipmData, float qdc);
// float alignQDC(SSiPMHit *sipmData, float qdc){
//     SCalContainer<6>* pSiPMCalPar;
//     // get TP calibrator parameters
//     pSiPMCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FibersTOFPETCalibratorPar"));
// 
//     if (!pSiPMCalPar)
//     {
//         std::cerr << "Parameter container 'SiPMsTOFPETCalibratorPar' was not obtained!" << std::endl;
//         exit(EXIT_FAILURE);
//     }
//     
// //     float SFibersRawClusterFinder::alignQDC(identifiedFiberDataC address, float qdc)
// // {
//     int mod,lay,el;
//     char *side;
//     sipmData->getAddress(mod,lay,el,*side);
//     SFibersChannel chan;
//     chan.m = mod;
//     chan.l = lay;
//     chan.s = el;
//     chan.side = side[0];
//     
//     auto _cpar = pSiPMCalPar->getPar(&chan);
//     auto&& cpar = *_cpar;
//     
//     if(cpar[0]==-100)
//     {
//         std::cout<<" Error: cpar[0]=-100.0" << std::endl;
//         return -100;
//     }
//     else 
//     {
//         return qdc*511./cpar[0]; 
//     }       
//     
// }

void SSiPMClusterFinder::print()
{
    int nclus = catSiPMsCluster->getEntries(); 
    
    std::cout << "\n" << "In SSiPMClusterFinder::" << __func__ << " " << std::endl;
    std::cout << "Found " << nclus << " clusters: " << std::endl;
    
    for(int c=0; c < nclus; ++c) // iterating over clusters
    {
       SSiPMCluster* pClus = dynamic_cast<SSiPMCluster*>(catSiPMsCluster->getObject(c));
       pClus->print();
    }
}

bool SSiPMClusterFinder::finalize()
{
    return true;
}
