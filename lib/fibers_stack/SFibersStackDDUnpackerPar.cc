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
    fBLMode = 0;
    fPolarity = 0;
    fAnaMode = 0;
    fIntMode = 0;
    fDeadTime = 0;
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

    TArrayI _b(16);
    if (!parcont->fill("fBLMode", _b)) return false;
    if (_v.GetSize() != 16)
    {
        std::cerr << "Size of fBLMode doesn't match 16" << std::endl;
        return false;
    }
    fBLMode = _b;

    if (!parcont->fill("fPolarity", fPolarity)) return false;
    if (!parcont->fill("fAnaMode", fAnaMode)) return false;
    if (!parcont->fill("fIntMode", fIntMode)) return false;
    if (!parcont->fill("fDeadTime", fDeadTime)) return false;

    if (!(fPolarity == 0 || fPolarity == 1))
    {
        std::cerr << "Incorrect value of fPolarity!" << std::endl;
        std::cerr << "Possible values are: 0 - NEGATIVE or 1 - POSITIVE" << std::endl;
        std::cerr << "fPolarity = " << fPolarity << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!(fAnaMode == 0 || fAnaMode == 1))
    {
        std::cerr << "Incorrect value of fAnaMode!" << std::endl;
        std::cerr << "Possible values are: 0 - Leading Edge or 1 - Constant Fraction" << std::endl;
        std::cerr << "fAnaMode = " << fAnaMode << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fIntMode < 0)
    {
        std::cerr << "fIntMode cannot be smaller than 0!" << std::endl;
        std::cerr << "Possible values are: 0 - TOT or >0 - Limit" << std::endl;
        std::cerr << "fIntMode = " << fIntMode << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fDeadTime < 0)
    {
        std::cerr << "fDeadTime cannot be smaller than 0!" << std::endl;
        std::cerr << "fDeadTime = " << fDeadTime << std::endl;
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
bool SFibersStackDDUnpackerPar::putParams(SParContainer* /*parcont*/) const { return true; }

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
    printf("\n Baseline modes =");
    for (int i = 0; i < 16; ++i)
        printf(" %d", fBLMode[i]);
    printf("\n polarity = %d\n", fPolarity);
    printf(" anamode = %d\n", fAnaMode);
    printf(" intmode = %d\n", fIntMode);
    printf(" deadtime = %d\n", fDeadTime);
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

Int_t SFibersStackDDUnpackerPar::getBLMode(Int_t chan) const
{
    if (chan < 0 or chan > 15) abort();
    return fBLMode[chan];
}
