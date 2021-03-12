// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersHitSim.h"

/**
 * \class SFibersHitSim
\ingroup lib_fibers

A container for Fibers Stack Hitibrated simulation data

*/

/**
 * Clear object.
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SFibersHitSim::Clear(Option_t* /*opt*/)
{
    fGeantEloss = 0.0;
    fGeantPoint.Clear();
}

/**
 * Print category.
 */
void SFibersHitSim::print() const
{
    SFibersHit::print();
    printf("    GEANT                 x,y,z= % 6.2f,% 6.2f,% 6.2f               Eloss= %7.2f\n",
           fGeantPoint.X(), fGeantPoint.Y(), fGeantPoint.Z(), fGeantEloss * 1000);
}
