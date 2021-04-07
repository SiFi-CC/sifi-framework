// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParSource.h"

#include <iostream>

/**
 * \class SParSource

\ingroup lib_base

Parameters Manager class. Stores and dumps all parameters used by the framework.

It's a singleton class of which object can be obtained using instance() method.

Paramaters mamager must be initializatied before
MDetectorManager::initParameterContainers() is called since it checks whether
the requested parameter containers exists.
*/

// for trim functions see
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

/**
 * Get plain container by name. Intepretation of the content must be done by appropriate class, see
 * SDatabase::getParContainer, SDatabase::getLookupContainer, SDatabase::getCalContainer for
 * examples.
 *
 * \param name container name
 * \return pointer to the container
 */
SContainer* SParSource::getContainer(const std::string& name)
{
    auto it = containers.find(name);
    if (it == containers.end()) { return nullptr; }

    return it->second.get();
}

void SParSource::print() const
{
    std::cout << "    Containers:" << std::endl;
    for (auto&& c : containers)
    {
        std::cout << "      + " << c.first << std::endl;
    }
}
