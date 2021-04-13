// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersCalSim.h"

#include <cstdio> // for printf

/**
 * \class SFibersCalSim
\ingroup lib_fibers

A container for Fibers Stack Calibrated simulation data

*/

/**
 * Clear object.
 * Parameter options are ignored, for ROOT compatibility.
 *
 * \param opt options
 */
void SFibersCalSim::Clear(Option_t* /*opt*/)
{
    fGeantEloss = 0.0;
    fGeantPoint.Clear();
}

/**
 * Print category.
 */
void SFibersCalSim::print() const
{
    SFibersCal::print();
    printf("  GEANT: position = %.2f,%.2f,%.2f  Eloss = %.2f\n", fGeantPoint.X(), fGeantPoint.Y(),
           fGeantPoint.Z(), fGeantEloss);
}
