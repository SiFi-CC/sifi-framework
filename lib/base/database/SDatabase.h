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

#ifndef SDATABASE_H
#define SDATABASE_H

#include "sifi_export.h"

#include "SContainer.h"
#include "SRun.h"

#include <algorithm> // for max
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class SPar;
class SParSource;
class SParContainer;
class SVirtualCalContainer;
class SLookupTable;

using cont_obj = std::variant<SPar*, SVirtualCalContainer*, SLookupTable*>;
using cont_obj_factory = std::function<cont_obj()>;

class SIFI_EXPORT SDatabase
{
public:
    enum class ContainerMode
    {
        None,
        Read,
        Create,
        Forward
    };

protected:
    std::vector<std::unique_ptr<SParSource>> sources; ///< Parameters source file
    std::unique_ptr<SParSource> target;               ///< Parameters destination file

    // Runs
    std::map<ulong, SRun> runs;

    // Technical containers
    struct ContainerData
    {
        std::string name;
        SParSource* source;
        ContainerMode mode;
        cont_obj_factory build_factory;
        std::shared_ptr<SContainer> cache;
        std::map<SRunsValidityRange, std::shared_ptr<SContainer>> validity;
    };

    std::map<std::string, ContainerData> container_data;

    std::string release; ///< stores parameters release name
    unsigned long current_run_id;
    unsigned long reference_run_id{0};

public:
    SDatabase();
    ~SDatabase();

    // methods
    /// Set parameters source
    /// \param source source file name
    void addSource(std::unique_ptr<SParSource>&& source);
    /// Set parameters destination
    /// \param target destination file name
    void setTarget(std::unique_ptr<SParSource>&& target);

    void writeTarget();
    void writeContainers(const std::vector<std::string>& names);
    void writeContainer(const std::string& name);

    bool addContainer(const std::string& name, cont_obj_factory&& f);
    SPar* getParContainer(const std::string& name);
    SLookupTable* getLookupContainer(const std::string& name);
    SVirtualCalContainer* getCalContainer(const std::string& name);

    auto getContainerMode(const std::string& name) -> std::optional<ContainerMode>;
    void setContainerMode(const std::string& name, ContainerMode mode)
    {
        container_data[name].mode = mode;
    }

    /// Set release value.
    /// \param r release name
    void setExperiment(std::string r) { release = r; }
    /// Get release value.
    /// \return release value
    auto getExperiment() const { return release; }

    constexpr void setCurrentRun(ulong run_id) { current_run_id = run_id; }
    constexpr void setReferenceRun(ulong run_id) { reference_run_id = run_id; }

    void initRun(ulong run_id);
    void initContainers(ulong run_id);
    /**
     * Init all containers belonging to the given release, and if release not given, abort
     * execution.
     */
    void initContainers();

    void print() const;

    friend class SParAsciiSource;

private:
    [[nodiscard]] auto getContainerSource(const std::string& name) -> SParSource*;
    [[nodiscard]] auto findContainerSource(const std::string& name) -> SParSource*;
    auto getContainer(const std::string& name) -> std::pair<SContainer*, bool>;
    auto getContainer(const std::string& name, ulong) -> std::pair<SContainer*, bool>;

    auto updateContainerModes() -> bool;

    auto getExperimentObjectFromSources() const -> std::optional<SExperiment>;

    auto findRun(ulong run_id) const -> std::optional<SRun>;
    auto getRun(ulong run_id) -> SRun;
    auto getRunFromSources(ulong run_id) const -> SRun;

    void initContainer(ulong run_id, ContainerData& cd);
};

/**
 * Helper class to provide unique interface for db whichever class is used. Could be as well
 * a namespace but then the mdb variable would be exposed.
 */
class SIFI_EXPORT SRuntimeDb
{
private:
    // constructors
    SRuntimeDb() = default;
    SRuntimeDb(SRuntimeDb const&) = delete;

    SRuntimeDb& operator=(SRuntimeDb const&) = delete;

    static SDatabase* mdb; ///< Instance of the SDatabase

public:
    static void init(SDatabase* db) { mdb = db; }
    static auto get() -> SDatabase* { return mdb; };
};

extern SIFI_EXPORT SDatabase* rdb();

auto operator"" _par(const char* name, std::size_t) -> SPar*;
auto operator"" _lookup(const char* name, std::size_t) -> SLookupTable*;
auto operator"" _cal(const char* name, std::size_t) -> SVirtualCalContainer*;

#endif /* SDATABASE_H */
