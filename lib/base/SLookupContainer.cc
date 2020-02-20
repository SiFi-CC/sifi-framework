// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>
#include <sstream>

#include "SLookupContainer.h"

/** \class SLookupContainer
\ingroup lib_base

SLookup is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SLookupManager and stored in the
SLookupContainer. The getParam() method reads content of the SLookupContainer and
fills variables inside the SLookup object. The putParam method allows to update
parameters in the container and write to param file.

\sa MFibersStackCalibratorPar
\sa MFibersStackDigitizerPar
\sa MFibersStackGeomPar
*/

/** Constructor
 * \param container container name
 */
SLookupContainer::SLookupContainer(const std::string& container)
    : container(container)
{
}

/** Print container
 */
void SLookupContainer::print()
{
    printf("Lookup Table [%s]\n", container.c_str());
    LookupVector::const_iterator it = lv.begin();
    for (; it != lv.end(); ++it)
    {
        printf("%s", it->c_str());
    }
}
