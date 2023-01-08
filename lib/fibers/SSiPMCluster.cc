// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/
 
#include "SSiPMCluster.h"
#include "SSiPMHit.h"
#include <cstdio>

/**
 * \class SSiPMCluster 
 * \ingroup lib_fibers
 * A container class representing cluster of SiPM hits. The cluster is formed
 * by SiPMs that responded within one event and are direct neighbours (touching
 * sides or corners).
 **/

void SSiPMCluster::Clear(Option_t* opt)
{
    point.Clear();
//    errors.Clear();
    hits.clear();
}

void SSiPMCluster::print() const
{
//    printf("SiPM CLUSTER:  module = %d  cluster = %d  x,y,z = (%f, %f, %f) +/- (%f, %f, %f)\n",
//           module, cluster, point.x(), point.y(), point.z(), errors.x(), errors.y(), errors.z());
    
//     printf("SiPM CLUSTER: cluster = %d num of hits = %d x,y,z = (%f, %f, %f)\n", cluster, hits.size(), point.x(), point.y(), point.z());
//     printf("SiPM HITS:\n");
    
    for(auto & h : hits) {
//        printf(" %d", h);
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(h));
        if (!pHit)
        {
            printf("SiPMHit %d doesn't exists!\n", h);
            continue;
        }
//         pHit->print();
    }
    
//     printf("\n");


}
