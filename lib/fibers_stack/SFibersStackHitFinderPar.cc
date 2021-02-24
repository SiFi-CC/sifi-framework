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
    fCalibPos.Reset();
    fCalibPosErr.Reset();
    fCalibEne.Reset();
    fCalibEneErr.Reset();
    fResPos.Reset();
    fResEne.Reset();
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
    if (!parcont->fill("fAlpha", fAlpha)) return false;

    if (!parcont->fill("fCalibPos", fCalibPos) ) return false;
    if (fCalibPos.GetSize() != numOfCalibPars)
    {
        std::cerr << "Size of fCalibPos doesn't match numOfCalibPars" << std::endl;
        return false;
    }

    if (!parcont->fill("fCalibPosErr", fCalibPosErr) ) return false;
    if (fCalibPosErr.GetSize() != numOfCalibPars)
    {
        std::cerr << "Size of fCalibPosErr doesn't match numOfCalibPars" << std::endl;
        return false;
    }

    if (!parcont->fill("fCalibEne", fCalibEne) ) return false;
    if (fCalibEne.GetSize() != numOfCalibPars)
    {
        std::cerr << "Size of fCalibEne doesn't match numOfCalibPars" << std::endl;
        return false;
    }

    if (!parcont->fill("fCalibEneErr", fCalibEneErr) ) return false;
    if (fCalibEneErr.GetSize() != numOfCalibPars)
    {
        std::cerr << "Size of fCalibEneErr doesn't match numOfCalibPars" << std::endl;
        return false;
    }

    if (!parcont->fill("fResPos", fResPos) ) return false;
    if (fResPos.GetSize() != numOfResPars)
    {
        std::cerr << "Size of fResPos doesn't match numOfResPars" << std::endl;
        return false;
    }

    if (!parcont->fill("fResEne", fResEne) ) return false;
    if (fResEne.GetSize() != numOfResPars)
    {
        std::cerr << "Size of fResEne doesn't match numOfResPars" << std::endl;
        return false;
    }
    return true;
}

/**
 * Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackHitFinderPar::putParams(SParContainer* parcont) const
{
    if (!parcont->add("fA0", fA0)) return false;
    if (!parcont->add("fLambda", fLambda)) return false;
    if (!parcont->add("fAlpha", fAlpha)) return false;

    return true;
}

/**
 * Print parameters
 */
void SFibersStackHitFinderPar::print() const
{
    //printf(" fA0 = %f\n", fA0);
    //printf(" fLambda = %f\n", fLambda);
    //printf(" fAlpha = %f\n", fLambda);
    printf(" fCalibPos:");
    for(int l=0; l < numOfCalibPars; ++l)
	    printf(" %f", fCalibPos[l]);
    printf("\n fCalibPosErr:");
    for(int l=0; l < numOfCalibPars; ++l)
	    printf(" %f", fCalibPosErr[l]);
    printf("\n fCalibEne:");
    for(int l=0; l < numOfCalibPars; ++l)
	    printf(" %f", fCalibEne[l]);
    printf("\n fCalibEneErr:");
    for(int l=0; l < numOfCalibPars; ++l)
	    printf(" %f", fCalibEneErr[l]);
    printf("\n fResPos:");
    for(int l=0; l < numOfResPars; ++l)
	    printf(" %f", fResPos[l]);
    printf("\n fResEne:");
    for(int l=0; l < numOfResPars; ++l)
	    printf(" %f", fResEne[l]);
    printf("\n");
}
