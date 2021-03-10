// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackClusterFinderPar.h"

#include "SParContainer.h"

#include <iostream>

/**
 * \class SFibersStackClusterFinderPar
\ingroup lib_fibers_stack

A container for Fibers Stack Calibrator parameters

\sa SPar

*/

/**
 * Clear parameters
 */
void SFibersStackClusterFinderPar::clear() { fClusterMode = 0; }

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackClusterFinderPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("fClusterMode", fClusterMode)) return false;

    return true;
}

/**
 * Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackClusterFinderPar::putParams(SParContainer* parcont) const
{
    if (!parcont->add("fClusterMode", fClusterMode)) return false;

    return true;
}

/**
 * Print parameters
 */
void SFibersStackClusterFinderPar::print() const { printf(" fClusterMode = %d\n", fClusterMode); }
