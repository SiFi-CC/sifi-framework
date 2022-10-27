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

#ifndef SParDatabaseSource_H
#define SParDatabaseSource_H

#include "sifi_export.h"

#include "SRESTInterface.h"
#include "SParSource.h"
#include "SRun.h"

#include <map>
#include <memory>
#include <string>

struct SContainer;

class SIFI_EXPORT SParDatabaseSource final : public SParSource
{
public:
    SParDatabaseSource();
    ~SParDatabaseSource() = default;

private:
    auto doSetOpenMode(SourceOpenMode /*mode*/) -> void override{};

    auto doFindContainer(const std::string& name) -> bool override;

    auto doGetContainer(const std::string& name, ulong runid)
        -> std::shared_ptr<SContainer> override;
    auto doSetContainer(const std::string& name, SContainer&& cont) -> bool;

    auto doInsertContainer(const std::string& name, SContainer* cont) -> bool override;

    auto doInsertContainer(const std::string& name, std::vector<SContainer*> cont) -> bool override;

    auto doGetRuns() -> std::vector<SRun> override;
    auto doGetRun(ulong runid) -> SRun override;
    virtual auto insertRun(SRun run) -> bool override;

    auto doGetExperiment() const -> std::optional<SExperiment> override;

    void doPrint() const override;

    auto canAcceptRuns() -> bool override { return true; }
    auto openRunContainer(int run_type, std::time_t start_time, std::string file_name) -> std::optional<SRun> override;
    auto closeRunContainer(std::time_t stop_time) -> std::optional<SRun> override;

    bool parseSource();

private:
    std::string dbapi;
    std::string token;

    std::unique_ptr<SRESTInterface> mysqlcon;

    std::optional<SExperiment> release;
};

#endif /* SParDatabaseSource_H */
