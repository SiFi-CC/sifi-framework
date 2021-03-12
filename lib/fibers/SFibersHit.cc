// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersHit.h"

/**
 * \class SFibersHit
\ingroup lib_fibers

A container for Fibers Stack Raw data

*/

/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SFibersHit::Clear(Option_t* /*opt*/)
{
    module = -1;
    layer = -1;
    fiber = -1;

    point.Clear();
    errors.Clear();
    E = sE = 0.;
    t = st = 0.;
}

/**
 * Print category
 */
void SFibersHit::print() const
{
    printf("FIBER HIT m,l,f=%2d,%2d,%2d  x,y,z= % 6.2f,% 6.2f,% 7.2f (%4.2f,%4.2f,%4.2f)  "
           "E= %7.2f +- %5.2f  u= % 6.2f (%4.2f)  Time= %4.2f +- %4.2f\n",
           module, layer, fiber, point.x(), point.y(), point.z(), errors.x(), errors.y(),
           errors.z(), E, sE, u, su, t, st);
}
