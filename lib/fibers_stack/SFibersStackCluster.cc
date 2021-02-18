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
 *
 * \param opt options
 */
void SFibersStackCluster::Clear(Option_t* opt)
{
    SFibersStackHit::Clear(opt);
    hits.clear();
}

/**
 * Print category
 */
void SFibersStackCluster::print() const
{
//     printf("CLUSTER HIT m,l,f=%d,%d,%d  x,y,z= %f,%f,%f (%f,%f,%f)  E= %f +- %f  Time= %f +- %f\n",
//            module, layer, fiber, x, y, z, sx, sy, sz, E, sE, t, st);
//     printf("       HITS:");
//     for (auto& h : hits)
//         printf(" %d", h);
//     printf("\n");
}
