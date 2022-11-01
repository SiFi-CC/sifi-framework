// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SDatabase.h"
#include "SCalContainer.h"
#include "SLookup.h"
#include "SPar.h"
#include "SParContainer.h"
#include "SParSource.h"
#include "SiFi.h"

#include <magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <tabulate/table.hpp>

#include <algorithm>
#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <iostream>
#include <iterator>
#include <utility> // for pair, make_pair, move

/**
 * \class SDatabase

\ingroup lib_base

Parameters Manager class. Stores and dumps all parameters used by the framework.

It's a singleton class of which object can be obtained using instance() method.

Paramaters mamager must be initializatied before
MDetectorManager::initParameterContainers() is called since it checks whether
the requested parameter containers exists.
*/

// for trim functions see
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

SDatabase* SRuntimeDb::mdb = nullptr;

/**
 * Shortcut
 * \return SDatabase instance
 */
SDatabase* rdb() { return SRuntimeDb::get(); }

/**
 * See
 * https://stackoverflow.com/questions/9954518/stdunique-ptr-with-an-incomplete-type-wont-compile#comment31948555_9954553
 */
SDatabase::SDatabase() = default;
SDatabase::~SDatabase() = default;

// methods
/// Set parameters source
/// \param source source file name
void SDatabase::addSource(std::unique_ptr<SParSource>&& source)
{
    source->setOpenMode(SourceOpenMode::Input);
    sources.push_back(std::move(source));
}
/// Set parameters destination
/// \param target destination file name
void SDatabase::setTarget(std::unique_ptr<SParSource>&& target)
{
    target->setOpenMode(SourceOpenMode::Output);
    this->target = std::move(target);
}

/**
 * Write all containers to destination file. Internally it creates a list of
 * containers and calls writeContainers();
 */
void SDatabase::writeTarget()
{
    auto search_fun = [](const auto& it1, const auto& it2) -> auto
    {
        return std::find_if(it1, it2,
                            [&](auto pair)
                            {
                                return pair.second.mode == ContainerMode::Create or
                                       pair.second.mode == ContainerMode::Forward;
                            });
    };

    std::vector<std::string> names;
    auto end = std::end(container_data);
    auto it = search_fun(std::begin(container_data), end);
    while (it != end)
    {
        names.push_back(it->first);
        it = search_fun(std::next(it), end);
    }

    //     std::copy(names.begin(), names.end(), std::ostream_iterator<std::string>(std::cout, "
    //     "));
    writeContainers(names);
}

/**
 * Write selected containers to the destination file.
 *
 * \param names vector of container names
 */
void SDatabase::writeContainers(const std::vector<std::string>& /*names*/)
{
    //     for (const auto& pc : obj_par_cont)    FIXME x3
    //         pc.second->toContainer();

    //     for (const auto& cc : obj_cal_cont)
    //         cc.second->toContainer();

    //     for (const auto& lc : obj_lu_cont)
    //         lc.second->toContainer();

    if (!target)
    {
        std::cerr << "No target specified!" << std::endl;
        abort();
    }

    for (auto& r : runs)
        target->insertRun(r.second);

    for (auto& c : container_data)
    {
        std::vector<SContainer*> v;
        v.reserve(c.second.validity.size());

        std::transform(c.second.validity.begin(), c.second.validity.end(), std::back_inserter(v),
                       [](const auto& p) { return p.second.get(); });

        std::cout << "Writing " << c.first << " with " << v.size() << " elements.\n";
        target->insertContainer(c.first, v);
    }
}

void SDatabase::writeContainer(const std::string& name)
{
    //     // Find mode for the container. If mode is None, then return nothing.
    //     auto mode_it = container_mode.find(name);
    //
    //     auto mode = ContainerMode::None;
    //
    //     if (mode_it == container_mode.end()) return;
    //
    //     mode = ContainerMode::Read;
    //
    //     auto it = obj_cal_cont.find(name);
    //     if (it != obj_cal_cont.end())
    //     {
    //
    //     }
    //
    //     // Read container from the source
    //     if (mode == ContainerMode::Forward)
    //     {
    //         auto source = container_source[name];
    //         if (source != nullptr)
    //         {
    //             auto cont = source->getContainer(name, current_run_id);
    //             cached[name] = cont;
    //             return {cont, true};
    //         }
    //     }
    //
    //     // Create new container in the cache
    //     if (mode_it->second == ContainerMode::Create)
    //     {
    //         auto source = container_source[name];
    //         if (source == nullptr)
    //         {
    //             auto cont = new SContainer();
    //             cached[name] = cont;
    //             return {cont, true};
    //         }
    //     }
    //
    //     return {nullptr, true};
}

/**
 * Add new parameter container.
 *
 * \param name container name
 * \param par parameter container object
 * \return success
 */
bool SDatabase::addContainer(const std::string& name, cont_obj_factory&& f)
{
    auto it = container_data.find(name);
    if (it != container_data.end())
    {
        spdlog::info("[{0}] Container {1} already added!", __func__, name);
        return false;
    }

    container_data[name] = {.name = name,
                            .source = nullptr,
                            .mode = ContainerMode::Read,
                            .build_factory = std::move(f),
                            .cache = {},
                            .validity = {}};

    return true;
}

auto SDatabase::getContainerMode(const std::string& name) -> std::optional<ContainerMode>
{
    if (auto const& it = container_data.find(name); it != container_data.cend())
        return it->second.mode;

    return {};
}

/**
 * Get parameter container by name.
 *
 * \param name container name
 * \return pointer to container
 */
SPar* SDatabase::getParContainer(const std::string& name)
{
    // If SPar was not registerd before, exit with failure.
    auto it = container_data.find(name);
    if (it == container_data.end())
    {
        spdlog::critical("[{0}] Parameter container {1} not known!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    SPar* par = nullptr;
    try
    {
        par = std::get<SPar*>(it->second.build_factory());
    }
    catch (const std::bad_variant_access& ex)
    {
        std::cout << ex.what() << '\n';
        return nullptr;
    };

    auto [cont, reinit] = getContainer(name);
    if (cont)
    {
        if (reinit)
        {
            SParContainer* pc = new SParContainer(name);
            pc->fromContainer(cont);
            //             obj_par_cont.emplace(name, pc); FIXME
            par->getParams(pc);
        }
        return par;
    }
    else
    {
        spdlog::critical("[{0}] Parameter container {1} could not be initialized!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    return par;
}

/**
 * Get lookup table by name.
 *
 * \param name container name
 * \return pointer to container
 */
SLookupTable* SDatabase::getLookupContainer(const std::string& name)
{
    // If SLookupTable was not registerd before, exit with failure.
    auto it = container_data.find(name);
    if (it == container_data.end())
    {
        spdlog::critical("[{0}] Lookup table {1} not known!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    SLookupTable* lu = nullptr;
    try
    {
        lu = std::get<SLookupTable*>(it->second.build_factory());
    }
    catch (const std::bad_variant_access& ex)
    {
        std::cout << ex.what() << '\n';
        return nullptr;
    };

    auto [cont, reinit] = getContainer(name);
    if (cont)
    {
        if (reinit) lu->fromContainer(cont);

        return lu;
    }
    else
    {
        spdlog::critical("[{0}] Lookup table {1} could not be initialized from sources!", __func__,
                         name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    return lu;
}

/**
 * Get calibration container by name.
 *
 * \param name container name
 * \return pointer to container
 */
SVirtualCalContainer* SDatabase::getCalContainer(const std::string& name)
{
    // If SLookupTable was not registerd before, exit with failure.
    auto it = container_data.find(name);
    if (it == container_data.end())
    {
        spdlog::critical("[{0}] Calibration container {1} not known!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);

        return nullptr;
    }

    SVirtualCalContainer* cal = nullptr;
    try
    {
        cal = std::get<SVirtualCalContainer*>(it->second.build_factory());
    }
    catch (const std::bad_variant_access& ex)
    {
        std::cout << ex.what() << '\n';
        return nullptr;
    };

    auto [cont, reinit] = getContainer(name);
    if (cont)
    {
        if (reinit) cal->fromContainer(cont);
        return cal;
    }
    else
    {
        spdlog::critical("[{0}] Calibration container {1} could not be initialized from sources!",
                         __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    return cal;
}

void SDatabase::initRun(ulong run_id)
{
    for (auto& source : sources)
    {
        if (run_id == 0)
        {
            auto res = source->getRuns();
            for (auto& r : res)
            {
                runs.insert({r.id, r});
            }
        }
        else
        {
            auto res = source->getRun(run_id);
            runs.insert({res.id, res});
        }
    }
}

auto SDatabase::updateContainerModes() -> bool
{
    if (!sources.size())
    {
        spdlog::warn("[{0}] No sources defined, turn all READ container modes into NONE.",
                     __func__);

        for (auto& cd : container_data)
        {
            if (cd.second.mode == ContainerMode::Read)
            {
                spdlog::info("[{0}] mark {1} as NONE mode.", __func__, cd.first);
                cd.second.mode = ContainerMode::None;
            }
            else if (cd.second.mode == ContainerMode::Forward)
            {
                spdlog::error("[{0}] Forward mode for {1} must have source.", __func__, cd.first);
            }
        }
        return true;
    }
    return false;
}

/**
 * \details
 * Detailed one
 */
void SDatabase::initContainers()
{
    auto rel_obj = getExperimentObjectFromSources();
    SExperiment rel = rel_obj.has_value() ? rel_obj.value() : SExperiment{};

    if (updateContainerModes()) return;

    initRun(0);
    // for (auto i = rel.first_run; i <= rel.last_run; ++i)
    for (auto& r : runs)
    {
        int i = r.first;

        for (auto& cd : container_data)
        {
            initContainer(i, cd.second);

            if (cd.second.mode != ContainerMode::None)
            {
                if (cd.second.source)
                {
                    auto cont = cd.second.source->getContainer(cd.first, i);

                    if (cont) cd.second.validity.insert({cont->validity, cont});
                }
            }
        }
    }
}

/**
 * \details
 * Loop over all preset containers modes, and fetch those which do not have None flag.
 * When fetching, find container source as well.
 * Not fetched containers can be eventually fetch later.
 */
void SDatabase::initContainers(ulong run_id)
{
    SExperiment rel;

    auto rel_obj = getExperimentObjectFromSources();
    if (rel_obj.has_value()) rel = rel_obj.value();

    // initRun(run_id);

    if (updateContainerModes()) return;

    for (auto& cd : container_data)
    {
        initContainer(run_id, cd.second);
    }
}

void SDatabase::initContainer(ulong run_id, ContainerData& cd)
{
    if (cd.mode != ContainerMode::None)
    {
        if (cd.source == nullptr)
        {
            cd.source = findContainerSource(cd.name);

            // If not found, at least cannot have mode Read or Forward, mode Write allows for
            // no container in the input.
            if (!cd.source and
                (cd.mode == ContainerMode::Read or cd.mode == ContainerMode::Forward))
            {
                spdlog::error("[{0}] Container {1} missing in the sources.", __func__, cd.name);
                std::abort();
            }
        }
    }
}

auto SDatabase::getContainerSource(const std::string& name) -> SParSource*
{
    // Check if Container was initialized from source, and if not, do it.
    auto it = container_data.find(name);

    if (it != container_data.end()) return it->second.source;

    return nullptr;
}

auto SDatabase::findContainerSource(const std::string& name) -> SParSource*
{
    for (auto& s : sources)
    {
        auto c = s->findContainer(name);
        if (c) { return s.get(); }
    }
    return nullptr;
}

auto SDatabase::getContainer(const std::string& name) -> std::pair<SContainer*, bool>
{
    return getContainer(name, current_run_id);
}
/**
 * Get plain container for given run id. First check whether it is cached and has valid run, then
 * return cached. Othwerwise search in source for new valid range if source provides it;
 *
 * \param name container name
 * \return valid container pointer
 */
auto SDatabase::getContainer(const std::string& name, ulong run_id) -> std::pair<SContainer*, bool>
{
    /**
     * Get container by name. First check whetehr it is cached. Cache is ether
     * a local copy of source-based container or the only instance of the container.
     * If cache does not exist, make action depending on the mode which is assigned
     * to the container:
     *  - for Raad or Forward, obtain the container from physical source, and put into cache
     *  - for Create, put container into cache
     */

    // If it was the same version like before, return cached one
    auto cd = container_data.find(name);
    if (cd != container_data.end() and cd->second.cache and
        cd->second.cache->validity.inside(run_id))
        return {cd->second.cache.get(), false};

    // Read container from the source
    if (cd->second.mode == ContainerMode::Read or cd->second.mode == ContainerMode::Forward)
    {
        auto source = cd->second.source;
        if (source != nullptr)
        {
            auto cont = source->getContainer(name, run_id);
            cd->second.validity.insert({cont->validity, cont});
            cd->second.cache = cont;
            return {cont.get(), true};
        }

        return {nullptr, true};
    }

    // Create new container in the cache
    if (cd->second.mode == ContainerMode::Create)
    {
        if (cd->second.source == nullptr)
        {
            auto cont = std::make_shared<SContainer>();
            cont->SetName(name.c_str());
            cd->second.validity.insert({cont->validity, cont});
            cd->second.cache = cont;
            return {cont.get(), true};
        }

        return {nullptr, true};
    }

    return {nullptr, true};
}

auto SDatabase::getExperimentObjectFromSources() const -> std::optional<SExperiment>
{
    for (auto& source : sources)
    {
        auto r = source->getExperiment();
        if (r.has_value()) { return *r; }
    }

    return std::nullopt;
}

auto SDatabase::findRun(ulong run_id) const -> std::optional<SRun>
{
    // Check if Container was initialized from source, and if not, do it.
    auto run_iter = runs.find(run_id);
    if (run_iter != runs.end()) return run_iter->second;

    return {};
}

auto SDatabase::getRun(ulong run_id) -> SRun
{
    auto r = findRun(run_id);
    if (!r) r = getRunFromSources(run_id);

    if (r)
    {
        runs.insert({r.value().id, r.value()});
        return r.value();
    }

    return {};
}

auto SDatabase::getRunFromSources(ulong run_id) const -> SRun
{
    for (auto& s : sources)
    {
        auto c = s->getRun(run_id);
        if (c.id == run_id) { return c; }
    }

    return {};
}

/**
 * Print containers
 */
void SDatabase::print() const
{
    std::cout << "\n";
    std::cout << "Container maps sizes\n";
    tabulate::Table map_mult_summary;
    map_mult_summary.add_row({"Sources", "Container sources", "Modes", "Factories", "Runs"});

    // map_mult_summary.add_row({std::to_string(sources.size()),
    //                           std::to_string(container_source.size()),
    //                           std::to_string(container_mode.size()),
    //                           std::to_string(obj_factory.size()), std::to_string(runs.size())});

    std::cout << map_mult_summary << "\n\n";

    std::cout << "Containers summary\n";
    tabulate::Table cont_summary;
    cont_summary.add_row({"Name", "Source", "Mode"});
    for (auto& cd : container_data)
    {
        auto src_ptr = cd.second.source;
        auto mode = cd.second.mode;

        char hex_string[20];
        sprintf(hex_string, "%#lx", (unsigned long)src_ptr); // convert number to hex

        auto mode_str = magic_enum::enum_name(mode);
        cont_summary.add_row({cd.first, hex_string, std::string(mode_str)});
    }
    std::cout << cont_summary << "\n";

    std::cout << "=== Runtime Database Summary ===\n";

    //     for (const auto& p : sources)
    //         p->print();
}

auto operator"" _par(const char* name, std::size_t) -> SPar*
{
    return SRuntimeDb::get()->getParContainer(name);
}

auto operator"" _lookup(const char* name, std::size_t) -> SLookupTable*
{
    return SRuntimeDb::get()->getLookupContainer(name);
}

auto operator"" _cal(const char* name, std::size_t) -> SVirtualCalContainer*
{
    return SRuntimeDb::get()->getCalContainer(name);
}
