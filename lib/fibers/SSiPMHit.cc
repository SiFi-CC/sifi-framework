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
    module=-1;
    layer=-1;
    element=-1;
    
    swSiPMID = -1;
     
    qdc = 0.;
    time = 0;
    
    hitID=-1;
    fX=-1000.;
    fY=-1000.;
    fZ=-1000.;
}

/**
 * Print category
 */
void SSiPMHit::print() const
{
    printf("module=%d layer=%d element=%d swSiPMID=%d  side=%c QDC=%f  Time=%lld fX=%f fY=%f fZ=%f\n", module, layer, element, swSiPMID, side, qdc, time, fX, fY, fZ);
}
