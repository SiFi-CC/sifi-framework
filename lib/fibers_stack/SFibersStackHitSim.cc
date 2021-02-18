// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackHitSim.h"

/**
 * \class SFibersStackHitSim
\ingroup lib_fibers_stack

A container for Fibers Stack Hitibrated simulation data

*/

/**
 * Clear object.
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SFibersStackHitSim::Clear(Option_t* /*opt*/)
{
    fGeantEloss = 0.0;
    fGeantPoint.Clear();
}

/**
 * Print category.
 */
void SFibersStackHitSim::print() const
{
    SFibersStackHit::print();
    printf("  GEANT: position = %.2f,%.2f,%.2f  Eloss = %.2f\n",
           fGeantPoint.X(), fGeantPoint.Y(), fGeantPoint.Z(), fGeantEloss);
}
