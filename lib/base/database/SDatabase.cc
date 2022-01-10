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

#include <algorithm>
#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <iostream>
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
void SDatabase::addSource(SParSource* source)
{
    source->setOpenMode(SourceOpenMode::Input);
    sources.push_back(source);
}
/// Set parameters destination
/// \param target destination file name
void SDatabase::setTarget(SParSource* target)
{
    target->setOpenMode(SourceOpenMode::Output);
    this->target = target;
}

/**
 * Write all containers to destination file. Internally it creates a list of
 * containers and calls writeContainers();
 */
void SDatabase::writeDestination()
{
    //     std::vector<std::string> names; FIXME
    //     names.reserve(containers.size());
    //
    //     for (auto& p : parconts)
    //     {
    //         SParContainer* pc = par_containers[p.first];
    //         p.second->putParams(pc);
    //     }
    //
    //     std::transform(containers.begin(), containers.end(), std::back_inserter(names),
    //                    [](std::pair<std::string, SContainer*> const& e) { return e.first; });
    //
    //     writeContainers(names);
}

/**
 * Write selected containers to the destination file.
 *
 * \param names vector of container names
 */
void SDatabase::writeContainers(const std::vector<std::string>& names)
{
    //     for (const auto& pc : par_containers)    FIXME x3
    //         pc.second->toContainer();

    //     for (const auto& cc : cal_containers)
    //         cc.second->toContainer();

    //     for (const auto& lc : lu_containers)
    //         lc.second->toContainer();

    if (!target)
    {
        std::cerr << "No target specified!" << std::endl;
        abort();
    }

    //     std::ofstream ofs(destination); FIXME
    //     if (ofs.is_open())
    //     {
    //         for (auto& c : names)
    //         {
    //             ofs << "[" << c << "]" << std::endl;
    //             for (const auto& l : containers.at(c)->lines)
    //                 ofs << l << std::endl;
    //         }
    //     }
    //     ofs.close();
}

/**
 * Print containers
 */
void SDatabase::print() const
{
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
    auto it = obj_factory.find(name);
    if (it != obj_factory.end())
    {
        spdlog::info("[{0}] Container {1} already added!", __func__, name);
        return false;
    }

    obj_factory.insert(std::make_pair(name, std::move(f)));
    container_mode[name] = ContainerMode::Read;

    return true;
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
    auto it = obj_factory.find(name);
    if (it == obj_factory.end())
    {
        spdlog::critical("[{0}] Parameter container {1} not known!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    SPar* par = nullptr;
    try
    {
        par = std::get<SPar*>(it->second());
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
    auto it = obj_factory.find(name);
    if (it == obj_factory.end())
    {
        spdlog::critical("[{0}] Lookup table {1} not known!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    SLookupTable* lu = nullptr;
    try
    {
        lu = std::get<SLookupTable*>(it->second());
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
    auto it = obj_factory.find(name);
    if (it == obj_factory.end())
    {
        spdlog::critical("[{0}] Calibration container {1} not known!", __func__, name);
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);

        return nullptr;
    }

    SVirtualCalContainer* cal = nullptr;
    try
    {
        cal = std::get<SVirtualCalContainer*>(it->second());
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

void SDatabase::initRun(ulong runid)
{
    for (auto& source : sources)
    {
        if (runid == 0)
        {
            auto res = source->getRuns();
            for (auto& r : res)
            {
                runs.insert({r.runid, r});
            }
        }
        else
        {
            auto res = source->getRun(runid);
            runs.insert({res.runid, res});
        }
    }
}

auto SDatabase::updateContainerModes() -> bool
{
    if (!sources.size())
    {
        spdlog::warn("[{0}] No sources defined, turn all READ container modes into NONE.",
                     __func__);

        for (auto& cm : container_mode)
        {
            if (cm.second == ContainerMode::Read)
            {
                spdlog::info("[{0}] mark {1} as NONE mode.", __func__, cm.first);
                cm.second = ContainerMode::None;
            }
            else if (cm.second == ContainerMode::Forward)
            {
                spdlog::error("[{0}] Forward mode for {1} must have source.", __func__, cm.first);
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

    for (auto i = rel.first_run; i <= rel.last_run; ++i)
    {
        initRun(i);

        for (auto& cm : container_mode)
        {
            if (cm.second != ContainerMode::None)
            {
                auto cs = container_source[cm.first];
                if (cs == nullptr)
                {
                    cs = findContainerSource(cm.first);
                    if (cs) container_source[cm.first] = cs;

                    // If not found, at least cannot have mode Read or Forward, mode Write allows
                    // for no container in the input.
                    if (!cs and
                        (cm.second == ContainerMode::Read or cm.second == ContainerMode::Forward))
                    {
                        spdlog::critical("[{0}] Container {1} missing in the sources!", __func__,
                                         cm.first);
                        std::abort();
                    }
                }

                if (cs)
                {
                    auto cont = cs->getContainer(cm.first, i);

                    if (cont) containers[cm.first].insert({cont->validity, cont});
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
void SDatabase::initContainers(ulong runid)
{
    SExperiment rel;

    auto rel_obj = getExperimentObjectFromSources();
    if (rel_obj.has_value()) rel = rel_obj.value();

    initRun(0);
    //     initRuns(runid);

    if (updateContainerModes()) return;

    for (auto& cm : container_mode)
    {
        if (cm.second != ContainerMode::None)
        {
            auto cs = container_source[cm.first];
            if (cs == nullptr)
            {
                cs = findContainerSource(cm.first);
                if (cs) container_source[cm.first] = cs;

                // If not found, at least cannot have mode Read or Forward, mode Write allows for
                // no container in the input.
                if (!cs and
                    (cm.second == ContainerMode::Read or cm.second == ContainerMode::Forward))
                {
                    spdlog::error("[{0}] Container {1} missing in the sources.", __func__,
                                  cm.first);
                    std::abort();
                }
            }
        }
    }
}

auto SDatabase::getContainerSource(const std::string& name) -> SParSource*
{
    // Check if Container was initialized from source, and if not, do it.
    auto it = container_source.find(name);

    if (it != container_source.end()) return it->second;

    return nullptr;
}

auto SDatabase::findContainerSource(const std::string& name) -> SParSource*
{
    for (auto s : sources)
    {
        auto c = s->findContainer(name);
        if (c) { return s; }
    }
    return nullptr;
}

auto SDatabase::getContainer(const std::string& name) -> std::pair<SContainer*, bool>
{
    return getContainer(name, current_runid);
}
/**
 * Get plain container for given run id. First check whether it is cached and has valid run, then
 * return cached. Othwerwise search in source for new valid range if source provides it;
 *
 * \param name container name
 * \return valid container pointer
 */
auto SDatabase::getContainer(const std::string& name, ulong runid) -> std::pair<SContainer*, bool>
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
    auto last_it = cached.find(name);
    if (last_it != cached.end() and last_it->second->validity.inside(runid))
        return {last_it->second.get(), false};

    // Find mode for the container. If mode is None, then return nothing.
    auto mode_it = container_mode.find(name);

    //     auto mode = mode_it->second;
    auto mode = ContainerMode::None;

    if (mode_it == container_mode.end())
    {
        container_mode[name] = ContainerMode::Read;
        mode = ContainerMode::Read;
    }
    else
    {
        mode = mode_it->second;
        //         return {nullptr, true};
    }

    // Read container from the source
    if (mode == ContainerMode::Read or mode == ContainerMode::Forward)
    {
        auto source = container_source[name];
        if (source != nullptr)
        {
            auto cont = source->getContainer(name, runid);
            containers[name].insert({cont->validity, cont});
            cached[name] = cont;
            return {cont.get(), true};
        }

        return {nullptr, true};
    }

    // Create new container in the cache
    if (mode_it->second == ContainerMode::Create)
    {
        auto source = container_source[name];
        if (source == nullptr)
        {
            auto cont = std::make_shared<SContainer>();
            cont->SetName(name.c_str());
            containers[name].insert({cont->validity, cont});
            cached[name] = cont;
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
