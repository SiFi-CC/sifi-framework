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

#ifndef SMYSQLINTERFACE_H
#define SMYSQLINTERFACE_H

#include "sifi_export.h"

#include "SRunContainer.h"
#include "SContainer.h"

#include <algorithm> // for max
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class SIFI_EXPORT SMysqlInterface
{
public:
    // constructors
    SMysqlInterface() = delete;
    SMysqlInterface(std::string_view server, int port) {}; // adjust connection to reqiuirements, add other constructors if needed, like via socket or so
    SMysqlInterface(SMysqlInterface const&) = delete;

    SMysqlInterface& operator=(SMysqlInterface const&) = delete;

public:
    // destructor
    virtual ~SMysqlInterface() {};

    /// Call this after conenction to select which param release we are about to work on
    /// Maych change between different calls to database
    void setParamRelease(std::string_view release) { param_release = release; }

    // Implement these
    auto getRunContainer(long runid) -> SRunContainer;
    auto getRunContainers(long runid_min, long runid_max) -> TObjArray;
    void addRunContainer(SRunContainer && runcont);

    // get containers for given runid
    auto getContainer(std::string_view && name, long runid) -> SContainer {};
    // get containers for given runid from min to the latest available for given release
    auto getContainers(std::string_view && name, long runid_min) -> std::vector<SContainer> {};
    // get containers between min (weak) and max (strong)
    auto getContainers(std::string_view && name, long runid_min, long runid_max) -> std::vector<SContainer> {};

    /// Add new container to the database, by name. It must be later validated via web interface.
    /// \return success of the operation
    bool addContainer(std::string_view && name, SContainer && cont) {};

private:
    std::string_view param_release;
};

#endif /* SMYSQLINTERFACE_H */
