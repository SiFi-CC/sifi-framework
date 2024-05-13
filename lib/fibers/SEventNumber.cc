// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SEventNumber.h"

#include <cstdio>
/**
 * \class SEventNumber
\ingroup lib_fibers

A container for Fibers Stack Raw data

*/

/**
 * Clear object
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SEventNumber::Clear(Option_t* /*opt*/)
{
    eventNumber = -1;
}

/**
 * Print category
 */
void SEventNumber::print() const
{
    printf("SEventNumber: eventNumber=%i ", eventNumber);
}



