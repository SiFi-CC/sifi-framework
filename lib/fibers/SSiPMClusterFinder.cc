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
#include <numeric>

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
    
    if( (abs(lay_1 - lay_2) < 2) &&    // if two hits have neighbouring element and layer numbers they are neighbours
       (abs(ele_1 - ele_2) < 2) )
    {
        return true;
    }
    else
        return false;
}


bool SSiPMClusterFinder::execute()
{
    int nhits = catSiPMsHit->getEntries(); // number of hits in current event
    int nclus = 0;
    bool clusterIncremented = 0;
    SLocator loc(1);
    std::vector<SSiPMCluster*> clusters;
    std::vector<SSiPMHit*> sipmHits;
    std::vector<int> isAssigned;
    int nAssignedHits = 0;
//     std::cout << "nhits" << nhits << std::endl;
    for (int i = 0; i < nhits; ++i){ // loop over all hits in current event
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(i)); // getting a hit
        sipmHits.push_back(pHit);
        isAssigned.push_back(0);
    }

    SSiPMCluster *pClus = new SSiPMCluster(); 
    pClus->addHit(sipmHits[0]->getID()); // adding first hit to the first cluster
    isAssigned[0] = 1;
    clusters.push_back(pClus);
    nAssignedHits++;
    
    while(nAssignedHits < nhits){
        clusterIncremented = 0;
        if(nAssignedHits == nhits) break;
        std::vector<Int_t> hits = clusters[clusters.size()-1]->getHitsArray(); // get list of hits within a cluster
        int nhit_in_clus = hits.size();
//         std::cout << "clusters.size(): " << clusters.size() << std::endl;
        for(int i=0; i< nhits; i++){
            for(int j=0; j<nhit_in_clus; j++){ 
                SSiPMHit* pHit_in_clus = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(hits[j]));
//                 std::cout << "current pHit_in_clus ID\t" << pHit_in_clus->getID() << " QDC:\t" << pHit_in_clus->getQDC() << std::endl;
//                 std::cout << "current sipm\t" << sipmHits[i]->getID() << " QDC:\t" << sipmHits[i]->getQDC() << std::endl;
                if(isAssigned[i] == 0 && checkIfNeighbours(sipmHits[i], pHit_in_clus)){
                        clusters[clusters.size()-1]->addHit(sipmHits[i]->getID()); // if yes, then this hit should belong to that cluster
//                         std::cout << "nAssignedHits incremented - found a neighbour of ID: " << sipmHits[i]->getID() << std::endl;
                        isAssigned[i] = 1;
                        nAssignedHits++;
                        clusterIncremented = 1;
                        break;
                }
            }
//             std::cout << std::endl;
        }
        if(clusterIncremented==0){
            SSiPMCluster *pClus = new SSiPMCluster(); 
            std::vector<int>::iterator it;
            it = std::find( isAssigned.begin(), isAssigned.end(), 0 );
            if ( it != isAssigned.end() ){ // found unassigned SiPM
                int location = std::distance( isAssigned.begin(), it );
//                 std::cout << "Found unassigned SiPM at location " << location;
                pClus->addHit(sipmHits[location]->getID());
//                 std::cout << " SiPM ID:" << sipmHits[location]->getID() << std::endl;
                clusters.push_back(pClus);
                isAssigned[location] = 1;
                nAssignedHits++;
            }
//             else {// 1 not found
//                 std::cout << "\n\n0 not found. no unassigned hits left" << std::endl;
//                 //break;
//             }
            
        }
    }

//     //print Clusters and SiPMs content (debug only)
//     for(int m=0; m<sipmHits.size(); m++){
//         sipmHits[m]->print();
//     }
//     
//     for(int l=0;l<clusters.size(); l++){
//         clusters[l]->print();
//     }

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
        
//         Long64_t time = pHit_in_clus->getTime();
        Double_t time = pHit_in_clus->getTime();

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
//             std::cout << "SiPMClusterFinder: " /*<< std::setprecision(15)*/<< time << std::endl;

            charge += pHit_in_clus->getQDC(); // charge is determined as sum of all hits charges
            alignedCharge += pHit_in_clus->getAlignedQDC(); // aligned charge is determined as sum of all aligned hits charges
            
            position = position + pHit_in_clus->getQDC() * TVector3(e, 0, l); // position calculated with COG
        }
        
        position = 1./charge * position;

        clusters[c]->setTime(time);
//         std::cout << "clusters[c]->setTime(time) " /*<< std::setprecision(15)*/<< clusters[c]->getTime() << std::endl;
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
