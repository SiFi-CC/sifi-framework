// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackRaw.h"

#include <cstdio>

/**
 * \class SFibersStackRaw
\ingroup lib_fibers_stack

A container for Fibers Stack Raw data

*/


/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SFibersStackRaw::Clear(Option_t* /*opt*/)
{
    module = -1;
    layer = -1;
    fiber = -1;

    qdc_l = 0.0;
    time_l = 0.0;
    qdc_r = 0.0;
    time_r = 0.0;
}

/**
 * Print category
 */
void SFibersStackRaw::print() const
{
    printf("fiber m,l,f=%d,%d,%d  L: QDC=%f  Time=%f   R: QDC=%f  Time=%f\n",
           module, layer, fiber, qdc_l, time_l, qdc_r, time_r);
}
