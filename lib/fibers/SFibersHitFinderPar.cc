// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersHitFinderPar.h"

#include "SParContainer.h"

#include <cstdio> // for printf
#include <iostream>
#include <memory> // for allocator

/**
 * \class SFibersHitFinderPar
\ingroup lib_fibers

A container for Fibers Stack Calibrator parameters

\sa SPar

*/
/**
 * Number of parameters for the resolution fit, fixed to 3
 */
Int_t numOfResPars = 3;
/**
 * Clear parameters
 */
void SFibersHitFinderPar::clear()
{
    fA0 = 0.0;
    fLambda = 0.0;
    fResPos.Reset();
    fResEne.Reset();
}

/**
 * The resolution equations are typically f(x) = p0/x + p1/x^{1/2} + p2/x^{3/2}
 * from Poisson statistics.
 * Here we can set/get the parameters themselves
 */
Float_t SFibersHitFinderPar::getResPos(UInt_t i) const
{
    Float_t ret = 0.;
    if (i < fResPos.GetSize()) { ret = fResPos[i]; }
    else
    {
        std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
        return 0.;
    }
    return ret;
}

Float_t SFibersHitFinderPar::getResEne(UInt_t i) const
{
    Float_t ret = 0.;
    if (i < fResEne.GetSize()) { ret = fResEne[i]; }
    else
    {
        std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
        return 0.;
    }
    return ret;
}

void SFibersHitFinderPar::setResPos(UInt_t i, Float_t f)
{
    if (i < numOfResPars) { fResPos.SetAt(f, i); }
    else
    {
        std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
    }
}

void SFibersHitFinderPar::setResEne(UInt_t i, Float_t f)
{
    if (i < numOfResPars) { fResEne.SetAt(f, i); }
    else
    {
        std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
    }
}

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersHitFinderPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("fA0", fA0)) return false;
    if (!parcont->fill("fLambda", fLambda)) return false;
    if (!parcont->fill("fAlpha", fAlpha)) return false;
    if (!parcont->fill("fResPos", fResPos)) return false;
    if (fResPos.GetSize() != numOfResPars)
    {
        std::cerr << "Size of fResPos doesn't match numOfResPars" << std::endl;
        return false;
    }
    if (!parcont->fill("fResEne", fResEne)) return false;
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
bool SFibersHitFinderPar::putParams(SParContainer* parcont) const
{
    if (!parcont->add("fA0", fA0)) return false;
    if (!parcont->add("fLambda", fLambda)) return false;
    if (!parcont->add("fAlpha", fAlpha)) return false;
    return true;
}

/**
 * Print parameters
 */
void SFibersHitFinderPar::print() const
{
    printf(" fA0 = %f\n", fA0);
    printf(" fLambda = %f\n", fLambda);
    printf(" fAlpha = %f\n", fAlpha);
    printf(" fResPos:");
    for (int l = 0; l < fResPos.GetSize(); ++l)
        printf(" %f", fResPos[l]);
    printf("\n fResEne:");
    for (int l = 0; l < fResPos.GetSize(); ++l)
        printf(" %f", fResEne[l]);
    printf("\n");
}
