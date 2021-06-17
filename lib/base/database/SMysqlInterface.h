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

#include "SContainer.h"
#include "SRunContainer.h"

#include <algorithm> // for max
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class SIFI_EXPORT SMysqlInterface
{
private:
    std::string api_url;
    static constexpr const char* auth_token = "59bcc6db2f36933601bdce41cbc5535655f0021c";
    // auth_token might change on the server side.
public:
    // constructors
    SMysqlInterface() = delete;
    SMysqlInterface(std::string_view api_url); // adjust connection to reqiuirements, add other constructors if
                                 // needed, like via socket or so
    SMysqlInterface(SMysqlInterface const&) = delete;

    SMysqlInterface& operator=(SMysqlInterface const&) = delete;

public:
    // destructor
    virtual ~SMysqlInterface() = default;

    /// Call this after conenction to select which param release we are about to work on
    /// Maych change between different calls to database
    void setParamRelease(std::string_view release) { param_release = release; }

    // Implement these
    auto getRunContainer(long runid) -> std::optional<SRunContainer>;
    auto getRunContainers(long runid_min, long runid_max) -> TObjArray;
    void addRunContainer(SRunContainer&& runcont);

    auto getContainer(std::string_view&& name, long runid) -> std::optional<SContainer>;
    auto getContainers(std::string_view&& name, long runid_min) -> std::vector<SContainer>;
    auto getContainers(std::string_view&& name, long runid_min, long runid_max)
        -> std::vector<SContainer>;
    bool addContainer(std::string_view&& name, SContainer&& cont);

private:
    std::string param_release;
};

#endif /* SMYSQLINTERFACE_H */