// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersCluster.h"

/**
 * \class SFibersCluster
\ingroup lib_fibers

A container for Fibers Stack Raw data

*/

/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 */
void SFibersCluster::Clear(Option_t* /*opt*/)
{
    point.Clear();
    errors.Clear();
    hits.clear();
}

/**
 * Print category
 */
void SFibersCluster::print() const
{
    printf("FIBERS CLUSTER: mod=%d cluster=%d  x,y,z= %f,%f,%f (%f,%f,%f)\n", module, cluster,
           point.x(), point.y(), point.z(), errors.x(), errors.y(), errors.z());
    printf("          HITS:");
    for (auto& h : hits)
        printf(" %d", h);
    printf("\n");
}
