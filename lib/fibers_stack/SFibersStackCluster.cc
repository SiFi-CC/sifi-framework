// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackCluster.h"

/**
 * \class SFibersStackCluster
\ingroup lib_fibers_stack

A container for Fibers Stack Raw data

*/

/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 */
void SFibersStackCluster::Clear(Option_t* /*opt*/)
{
    point.Clear();
    errors.Clear();
    hits.clear();
}

/**
 * Print category
 */
void SFibersStackCluster::print() const
{
    printf("FIBERS CLUSTER: mod=%d cluster=%d  x,y,z= %f,%f,%f (%f,%f,%f)\n", module, cluster,
           point.x(), point.y(), point.z(), errors.x(), errors.y(), errors.z());
    printf("          HITS:");
    for (auto& h : hits)
        printf(" %d", h);
    printf("\n");
}
