// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SSiPMHit.h"

#include <cstdio>

/**
 * \class SSiPMHit
\ingroup lib_fibers

A container for Fibers Stack Raw data

*/

/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SSiPMHit::Clear(Option_t* /*opt*/)
{
    swSiPMID = -1;

    qdc = 0.0;
    time = 0;
}

/**
 * Print category
 */
void SSiPMHit::print() const
{
    printf("swSiPMID=%d  QDC=%f  Time=%lld\n", swSiPMID,
           qdc, time);
}
