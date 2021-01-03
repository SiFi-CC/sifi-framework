// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackHitFinderPar.h"

#include "SParContainer.h"

#include <iostream>

/**
 * \class SFibersStackHitFinderPar
\ingroup lib_fibers_stack

A container for Fibers Stack Calibrator parameters

\sa SPar

*/

/**
 * Clear parameters
 */
void SFibersStackHitFinderPar::clear()
{
    fA0 = 0.0;
    fLambda = 0.0;
}

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackHitFinderPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("fA0", fA0)) return false;
    if (!parcont->fill("fLambda", fLambda)) return false;

    return true;
}

/**
 * Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackHitFinderPar::putParams(SParContainer* /*parcont*/) const
{
    return true;
}

/**
 * Print parameters
 */
void SFibersStackHitFinderPar::print() const
{
    printf(" fA0 = %f\n", fA0);
    printf(" fLambda = %f\n", fLambda);
}
