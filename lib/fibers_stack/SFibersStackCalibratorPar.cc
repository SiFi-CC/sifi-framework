// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackCalibratorPar.h"

#include "SParContainer.h"

/** \class SFibersStackCalibratorPar
\ingroup lib_fibers_stack

A container for Fibers Stack Calibrator parameters

\sa SPar

*/

/** Clear parameters
 */
void SFibersStackCalibratorPar::clear()
{
    fAdcGain = 1.0;
    fAdcOffset = 0.0;
}

/** Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackCalibratorPar::getParams(SParContainer* parcont)
{
    if (!parcont->fill("fAdcGain", fAdcGain)) return false;
    if (!parcont->fill("fAdcOffset", fAdcOffset)) return false;
}

/** Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackCalibratorPar::putParams(SParContainer* parcont) const
{
}

/** Print parameters
 */
void SFibersStackCalibratorPar::print() const
{
    printf(" +++\n adc gain = %g\n", fAdcGain);
    printf(" adc offset = %g\n", fAdcOffset);
}
