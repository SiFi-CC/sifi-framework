/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SPARSOURCE_H
#define SPARSOURCE_H

#include "sifi_export.h"

#include <memory>
#include <string>
#include <vector>

class SContainer;

enum class SourceOpenMode
{
    Input,
    Output
};

class SIFI_EXPORT SParSource
{
public:
    virtual ~SParSource() = default;

    virtual auto setOpenMode(SourceOpenMode mode) -> void = 0;

    /**
     * Checke whether source provides given container name.
     *
     * \param name container name
     * \return true if found
     */
    virtual auto findContainer(const std::string& name) -> bool = 0;

    /**
     * Get plain container by name. Intepretation of the content must be done by appropriate class,
     * see SDatabase::getParContainer, SDatabase::getLookupContainer, SDatabase::getCalContainer for
     * examples.
     *
     * \param name container name
     * \param runid run id
     * \return pointer to the container
     */
    virtual auto getContainer(const std::string& name, ulong runid)
        -> std::shared_ptr<SContainer> = 0;

    virtual auto insertContainer(const std::string& name, std::vector<SContainer*> cont)
        -> bool = 0;

    /// Print containers stored in the source. Details about source must be print by subclass
    /// method.
    virtual void print() const;
};

#endif /* SPARSOURCE_H */
