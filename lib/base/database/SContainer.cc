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

#include <iostream> // for printf

/**
 * Print container content.
 *
 * \param name name to display
 */
void SContainer::print(std::string_view name) const
{
    std::cout << "# Container Table [" << name << "]\n";
    std::cout << "# -----\n";
    for (auto line : lines)
    {
        printf("%s\n", line.c_str());
    }
    std::cout << "# -----\n";
}
