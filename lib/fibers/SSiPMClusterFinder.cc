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
#include "SFibersGeomPar.h"
#include "SSiPMLookup.h"
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
    catSiPMsHit = sifi()->getCategory(SCategory::CatSiPMHit);
    
    if(!catSiPMsHit)
    {
        std::cerr << "No CatSiPMHit category!" << std::endl;
        return false;
    }
    
//    catSiPMsCluster = sifi()->getCategory(SCategory::CatSiPMClus);
//    
//    if(!catSiPMsCluster)
//    {
//        std::cerr << "No CatSiPMClus category!" << std::endl;
//    }
    
    pLookup = dynamic_cast<SSiPMLookupTable*>(pm()->getLookupContainer("SiPMLookupTable")); // TODO transfer this to STP4to1source?
    
    if(!pLookup)
    {
        std::cerr << "No lookup table SiPMLookupTable!" << std::endl;
        return false;
    }
    
    return true;
}

bool checkIfNeighbours(SSiPMHit* hit_1, SSiPMHit* hit_2)
{
    
    
}

bool SSiPMClusterFinder::execute()
{
    // getting geometry
    const size_t n_modules = pLookup->getModules();
    size_t n_layers[n_modules];
    size_t n_sipms_per_layer[n_modules];
    
    for(int i=0; i<n_modules; ++i)
    {
        n_layers[i] = pLookup->getLayers(i);
        n_sipms_per_layer[i] = pLookup->getSiPMsPerLayer(i, 0);
    }
    
    // temporary structure representing SiPM cluster
    struct tmp_cluster
    {
        int id{-1};
        TVector3 position;
        TVector3 error;
        std::vector<SSiPMHit*> hits;
    };
     
    // vector of all clusters in the analyzed event
    std::vector<tmp_cluster> clusters;
    
    // map of hits indicating which of hits belong to a cluster; int = -1 (no cluster assigned), int >= 0 (cluster id)
    std::map<SSiPMHit*, int> hit_cluster_map;
    
    int n_entries = catSiPMsHit->getEntries();
    
    // filling map of hits and assigning -1 to all hits
    for (int i = 0; i< n_entries; ++i)
    {
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(i));
        
        if(!pHit)
        {
            printf("SiPM hit doesn't exist!\n");
            continue;
        }
        
        hit_cluster_map[pHit] = -1;
    }
    
    // assigning hits to clusters
    int cluster_id = -1;
    int unassigned = hit_cluster_map.size();
    
    for (auto h = hit_cluster_map.begin(); h != hit_cluster_map.end(); ++h)
    {
        // if hit doesn't belong to any cluster, create one
        if (h->second == -1)
        {
            h->second = ++cluster_id;
            
            tmp_cluster cl;
            cl.id = cluster_id;
            cl.hits.push_back(h->first);
            
            clusters.push_back(cl);
            
            --unassigned;
        }
        else
            continue;
        
        // loop over remaining hits and try to assign them, repeat as many times as not assigned hits remains.
        // If touching SiPMs found aasign them to the same cluster and continue.
        
        for (int i = 0; i < unassigned; ++i)
        {
            
        }
    }
    
    return true;
}

bool SSiPMClusterFinder::finalize()
{
    return true;
}
