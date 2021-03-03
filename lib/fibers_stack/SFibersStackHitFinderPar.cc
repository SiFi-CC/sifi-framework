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
    fResPos.Reset();
    fResEne.Reset();
}
/**
 * The resolution equations are typically f(x) = p0/x + p1/x^{1/2} + p2/x^{3/2}
 * from Poisson statistics.
 * Here we can set/get the number of resolution parameters (default=3) and the
 * parameters themselves
 */
UInt_t SFibersStackHitFinderPar::getNumOfResPars() const
{
	return numOfResPars;
}

Float_t SFibersStackHitFinderPar::getResPos(UInt_t i) const
{
	Float_t ret = 0.;
	if(i < fResPos.GetSize() ) {
		ret = fResPos[i];
	} else {
        	std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
		return 0.;
	}
	return ret;
}

Float_t SFibersStackHitFinderPar::getResEne(UInt_t i) const
{
	Float_t ret = 0.;
	if(i < fResEne.GetSize() ) {
		ret = fResEne[i];
	} else {
        	std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
		return 0.;
	}
	return ret;
}

void SFibersStackHitFinderPar::setNumOfResPars(UInt_t i) {
	numOfResPars = i;
}

void SFibersStackHitFinderPar::setResPos(Float_t f, UInt_t i) {
	if(i < getNumOfResPars() ) {
		fResPos.SetAt(f, i);
	} else {
        	std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
	}
}

void SFibersStackHitFinderPar::setResEne(Float_t f, UInt_t i) {
	if(i < getNumOfResPars() ) {
		fResEne.SetAt(f, i);
	} else {
        	std::cerr << "TArrayF index is more than allowed in numOfResPars" << std::endl;
	}
}

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackHitFinderPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("numOfResPars", numOfResPars) ) return false;
    if (!parcont->fill("fA0", fA0)) return false;
    if (!parcont->fill("fLambda", fLambda)) return false;
    if (!parcont->fill("fAlpha", fAlpha)) return false;
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
    printf(" fA0 = %f\n", fA0);
    printf(" fLambda = %f\n", fLambda);
    printf(" fAlpha = %f\n", fAlpha);
    printf(" fResPos:");
    for(int l=0; l < fResPos.GetSize(); ++l)
	    printf(" %f", fResPos[l]);
    printf("\n fResEne:");
    for(int l=0; l < fResPos.GetSize(); ++l)
	    printf(" %f", fResEne[l]);
    printf("\n");
}
