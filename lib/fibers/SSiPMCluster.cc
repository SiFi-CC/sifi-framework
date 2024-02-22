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
    errors.Clear();
    hits.clear();
}

void SSiPMCluster::print() const
{
    
    printf("SiPM CLUSTER: clusterID = %d, num of hits = %ld, time = %f, QDC = %f,  alignedQDC = %f, x,y,z = (%f, %f, %f)\n", clusterID, hits.size(), time, qdc, aligned_qdc, point.x(), point.y(), point.z());
    printf("SiPM HITS: ");
    
    for(auto & h : hits)
    {
        printf("%i ", h);
    }
    
    printf("\n");

}
