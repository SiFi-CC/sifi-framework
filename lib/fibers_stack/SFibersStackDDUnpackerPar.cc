// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackDDUnpackerPar.h"

#include "SParContainer.h"

#include <iostream>

/**
 * \class SFibersStackDDUnpackerPar
\ingroup lib_fibers_stack

A container for Fibers Stack Calibrator parameters

\sa SPar

*/

/**
 * Clear parameters
 */
void SFibersStackDDUnpackerPar::clear()
{
    fThreshold = 0.0;
    fVetoThreshold = 0.0;
    nPolarity = 0;
    nAnaMode = 0;
    nIntMode = 0;
    nDeadTime = 0;
}

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackDDUnpackerPar::getParams(SParContainer* parcont)
{
    TArrayF _t(16);
    if (!parcont->fill("fThreshold", _t)) return false;
    if (_t.GetSize() != 16)
    {
        std::cerr << "Size of fThreshold doesn't match 16" << std::endl;
        return false;
    }
    fThreshold = _t;

    TArrayF _v(16);
    if (!parcont->fill("fVetoThreshold", _v)) return false;
    if (_v.GetSize() != 16)
    {
        std::cerr << "Size of fVetoThreshold doesn't match 16" << std::endl;
        return false;
    }
    fVetoThreshold = _v;
    
    if (!parcont->fill("nPolarity", nPolarity)) return false;
    if (!parcont->fill("nAnaMode", nAnaMode)) return false;
    if (!parcont->fill("nIntMode", nIntMode)) return false;
    if (!parcont->fill("nDeadTime", nDeadTime)) return false;

    if( ! (nPolarity == 0 || nPolarity ==1) )
    {
        std::cerr << "Incorrect value of nPolarity!" << std::endl;
        std::cerr << "Possible values are: 0 - NEGATIVE or 1 - POSITIVE" << std::endl;
        std::cerr << "nPolarity = " << nPolarity << std::endl;
        exit(EXIT_FAILURE);
    }

    if( ! (nAnaMode == 0 || nAnaMode ==1) )
    {
        std::cerr << "Incorrect value of nAnaMode!" << std::endl;
        std::cerr << "Possible values are: 0 - Leading Edge or 1 - Constant Fraction" << std::endl;
        std::cerr << "nAnaMode = " << nAnaMode << std::endl;
        exit(EXIT_FAILURE);
    }
    
    return true;
}

/**
 * Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackDDUnpackerPar::putParams(SParContainer* /*parcont*/) const
{
    return true;
}

/**
 * Print parameters
 */
void SFibersStackDDUnpackerPar::print() const
{
    printf(" +++\n Thresholds =");
    for (int i = 0; i < 16; ++i)
        printf(" %.2f", fThreshold[i]);
    printf("\n Veto thresholds =");
    for (int i = 0; i < 16; ++i)
        printf(" %.2f", fVetoThreshold[i]);
    printf("\n polarity = %d\n", nPolarity);
    printf(" anamode = %d\n", nAnaMode);
    printf(" intmode = %d\n", nIntMode);
    printf(" deadtime = %d\n", nDeadTime);
}

Float_t SFibersStackDDUnpackerPar::getThreshold(Int_t chan) const
{
    if (chan < 0 or chan > 15) abort();
    return fThreshold[chan];
}

Float_t SFibersStackDDUnpackerPar::getVetoThreshold(Int_t chan) const
{
    if (chan < 0 or chan > 15) abort();
    return fVetoThreshold[chan]; 
}
