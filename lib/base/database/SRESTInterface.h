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

#ifndef SRESTINTERFACE_H
#define SRESTINTERFACE_H

#include "sifi_export.h"

#include "SDbAbstractInterface.h"

#include "SContainer.h"
#include "SParDatabaseSource.h"
#include "SRun.h"

#include <algorithm> // for max
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class SIFI_EXPORT SRESTInterface final : public SDbAbstractInterface
{

    // auth_token might change on the server side.
public:
    // constructors
    /**
     * \param url REST api api_url
     * \param token authentication token
     */
    SRESTInterface();
    SRESTInterface(std::string url);
    SRESTInterface(SRESTInterface const&) = delete;

    SRESTInterface& operator=(SRESTInterface const&) = delete;

    // destructor
    virtual ~SRESTInterface() = default;

    void setUrl(std::string url) { api_url = url; }
    void setAuth(SIFI::Auth::auth_variant_t auth) { auth_data = auth; }

    auto connected() const -> bool override;

    /// Call this after conenction to select which param release we are about to work on
    /// Maych change between different calls to database
    void setExperiment(std::string exp) { experiment = exp; }

    auto getExperimentContainer(std::string name) -> std::optional<SExperiment> override;

    // Implement these
    auto getRunContainer(long runid) -> SRun override;
    auto getRunContainers(long runid_min, long runid_max) -> std::vector<SRun> override;
    auto openRunContainer(int run_type, std::time_t start_time, std::string file_name)
        -> std::optional<SRun> override;
    auto closeRunContainer(std::time_t stop_time) -> std::optional<SRun> override;

    auto findContainer(std::string name) -> bool override;
    auto getContainer(std::string name, long runid) -> std::optional<SContainer> override;
    auto getContainers(std::string name, long runid_min) -> std::vector<SContainer> override;
    auto getContainers(std::string name, long runid_min, long runid_max)
        -> std::vector<SContainer> override;
    bool addContainer(std::string name, SContainer&& cont) override;

private:
    std::string api_url;
    SIFI::Auth::auth_variant_t auth_data;

    bool connection_ok;
    std::string experiment;
};

#endif /* SRESTINTERFACE_H */