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

#ifndef SPARROOTSOURCE_H
#define SPARROOTSOURCE_H

#include "sifi_export.h"

#include "SParSource.h"

#include <map>
#include <string>

class SContainer;
struct validity_range_t;

class SIFI_EXPORT SParRootSource : public SParSource
{
public:
    SParRootSource(const std::string& source);
    SParRootSource(std::string&& source);
    virtual ~SParRootSource() = default;

    virtual SContainer* getContainer(const std::string& name, long runid) override;

    void print() const override;

private:
    bool parseSource();

private:
    std::map<std::string, std::map<validity_range_t, SContainer*>>
        containers;                                    ///< Containers mirrors
    std::map<std::string, SContainer*> last_container; ///< Last used container, for caching purpose
    std::string source;                                ///< Root file name
};

#endif /* SPARROOTSOURCE_H */
