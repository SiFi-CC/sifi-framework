// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackDigitizerPar.h"

#include "SParContainer.h"

/** \class SFibersStackDigitizerPar
\ingroup lib_fibers_stack

A container for Fibers Stack Digitizer parameters

\sa SPar

*/

/** Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackDigitizerPar::getParams(SParContainer* /*parcont*/)
{
    return true;
}

/** Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersStackDigitizerPar::putParams(SParContainer* /*parcont*/) const
{
    return true;
}

/** Clear parameters
 */
void SFibersStackDigitizerPar::clear()
{
}
