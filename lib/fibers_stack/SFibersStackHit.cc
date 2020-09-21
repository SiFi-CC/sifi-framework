// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackHit.h"

/**
 * \class SFibersStackHit
\ingroup lib_fibers_stack

A container for Fibers Stack Raw data

*/

/**
 * Constructor
 */
SFibersStackHit::SFibersStackHit() : TObject() {}

/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SFibersStackHit::Clear(Option_t* /*opt*/)
{
    module = -1;
    layer = -1;
    fiber = -1;

    x = y = z = 0.;
    sx = sy = sz = 0.;
    E = sE = 0.;
    t = st = 0.;
}

/**
 * Print category
 */
void SFibersStackHit::print() const
{
    printf("FIBER HIT m,l,f=%d,%d,%d  x,y,z= %f,%f,%f (%f,%f,%f)  E= %f +- %f  "
           "Time= %f +- %f\n",
           module, layer, fiber, x, y, z, sx, sy, sz, E, sE, t, st);
}
