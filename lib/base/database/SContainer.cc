// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SContainer.h"

/**
 * Print container content.
 */
void SContainer::print(std::string name) const
{
    printf("Container Table [%s]\n", name.c_str());
    for (auto line : lines)
    {
        printf("%s\n", line.c_str());
    }
}
