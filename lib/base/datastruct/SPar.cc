// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SPar.h"

#include "SParContainer.h"

/**
 * \class SPar
\ingroup lib_base

SPar is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SParManager and stored in the
SParContainer. The getParam() method reads content of the SParContainer and
fills variables inside the SPar object. The putParam method allows to update
parameters in the container and write to param file.

\sa SFibersCalibratorPar
\sa SFibersDigitizerPar
\sa SFibersGeomPar
*/

/**
 * Default constructor
 */
SPar::SPar() : parcont(nullptr) {}

/**
 * Print container info
 */
void SPar::print() const { parcont->print(); }
