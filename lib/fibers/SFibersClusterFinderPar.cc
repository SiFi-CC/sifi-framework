// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersClusterFinderPar.h"

#include "SParContainer.h"

#include <iostream>

/**
 * \class SFibersClusterFinderPar
\ingroup lib_fibers

A container for Fibers Stack Calibrator parameters

\sa SPar

*/

/**
 * Clear parameters
 */
void SFibersClusterFinderPar::clear() { fClusterMode = 0; }

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersClusterFinderPar::getParams(SParContainer* parcont)
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
bool SFibersClusterFinderPar::putParams(SParContainer* parcont) const
{
    if (!parcont->add("fClusterMode", fClusterMode)) return false;

    return true;
}

/**
 * Print parameters
 */
void SFibersClusterFinderPar::print() const { printf(" fClusterMode = %d\n", fClusterMode); }
