// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

/**
 * \class SFibersDigitizerPar
 \ i*ngroup lib_fibers

 A container for Fibers Stack Digitizer parameters

 \sa SPar

 */

#include "SFibersDigitizerPar.h"

class SParContainer;

/**
 * Get parameters
 * \sa SPar::getParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersDigitizerPar::getParams(SParContainer* /*parcont*/) { return true; }

/**
 * Put parameters
 * \sa SPar::putParams()
 * \param parcont pointer to container object
 * \return success
 */
bool SFibersDigitizerPar::putParams(SParContainer* /*parcont*/) const { return true; }

/**
 * Clear parameters
 */
void SFibersDigitizerPar::clear() {}
