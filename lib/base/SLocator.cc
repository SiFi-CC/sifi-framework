// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include <TClass.h>
#include <TClonesArray.h>

#include "SLocator.h"

/** \class SLocator
\ingroup lib_base

A class to provide multi-dimensional access to the address space of the data.

The data in the MCategory object are stored in linear array, though the
address space can be multidimensional and defined in the constructor.

The address of the slot of the data object is therefore deterimded by the
Locator which internally translates it into proper linear array distance.

The methods of the MCategory object are checking whether dimensionality of the
locator matches the dimensionality of the locator and returns null object of
dimensions don't fit, also appropriate message is printed to standard error
output.
*/

/** Print locator info.
 */
void SLocator::print() const
{
    printf("Locator: n=%ld => ", dim);
    for (int i = 0; i < dim; ++i)
        printf("  %ld", addr[i]);
    putchar('\n');
}
