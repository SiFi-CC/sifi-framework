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

#ifndef SPARASCIISOURCE_H
#define SPARASCIISOURCE_H

#include "sifi_export.h"

#include "SDatabase.h"
#include "SParSource.h"
#include "SRun.h"

#include <map>
#include <memory>
#include <string>

class SContainer;

class SIFI_EXPORT SParAsciiSource final : public SParSource
{
public:
    SParAsciiSource(const std::string& source);
    SParAsciiSource(std::string&& source);
    ~SParAsciiSource() = default;

private:
    auto doSetOpenMode(SourceOpenMode mode) -> void override {}

    auto doFindContainer(const std::string& name) -> bool override;

    auto doGetContainer(const std::string& name, ulong runid)
        -> std::shared_ptr<SContainer> override;

    auto doInsertContainer(const std::string& name, SContainer* cont) -> bool override;

    auto doInsertContainer(const std::string& name, std::vector<SContainer*> cont) -> bool override;

    auto doGetRuns() -> std::vector<SRun> override { return std::vector{dummy_run}; };
    auto doGetRun(ulong runid) -> SRun override { return dummy_run; }
    auto doInsertRun(SRun run) -> bool override
    {
        dummy_run = run;
        return true;
    };

    auto doGetExperiment() const -> std::optional<SExperiment> override
    {
        return SExperiment(SRuntimeDb::get()->getExperiment(), 0, 0);
    }

    void doPrint() const override;

    bool parseSource();

private:
    std::string source;                                            ///< ascii file name
    std::map<std::string, std::shared_ptr<SContainer>> containers; ///< Containers mirrors
    SRun dummy_run;
};

#endif /* SPARASCIISOURCE_H */
