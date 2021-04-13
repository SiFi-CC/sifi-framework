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

SDatabase* SDatabase::pm = nullptr;

/**
 * Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SDatabase* SDatabase::instance()
{
    if (!pm) pm = new SDatabase;

    return pm;
}

/**
 * Shortcut
 * \return SDatabase instance
 */
SDatabase* pm() { return SDatabase::instance(); }

/**
 * Destructor
 */
SDatabase::~SDatabase() { pm = nullptr; }

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
    for (const auto& p : par_containers)
        p.second->print();
}

/**
 * Add new parameter container.
 *
 * \param name container name
 * \param par parameter container object
 * \return success
 */
bool SDatabase::addParContainer(const std::string& name, std::unique_ptr<SPar>&& par)
{
    auto it = par_parameters.find(name);
    if (it != par_parameters.end()) return false;

    par_parameters.insert(std::make_pair(name, std::move(par)));
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
    auto it = par_parameters.find(name);
    if (it == par_parameters.end())
    {
        std::cerr << "Parameter container " << name << " not known!" << std::endl;
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    // TODO current id number
    long runid = 1e10;

    // Check if Container was initialized from source, and if not, do it.
    auto it2 = par_containers.find(name);
    if (it2 == par_containers.end())
    {
        for (auto s : sources)
        {
            auto c = s->getContainer(name, runid);
            if (c)
            {
                conts_sources.emplace(name, s);
                SParContainer* pc = new SParContainer(name);
                pc->fromContainer(c);
                par_containers.emplace(name, pc);
                it->second.get()->getParams(pc);
                return it->second.get();
            }
        }

        std::cerr << "Parameter container " << name << " could not be initialized!" << std::endl;
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    return it->second.get();
}

/**
 * Add new lookup table container.
 *
 * \param name container name
 * \param lu lookup table object
 * \return success
 */
bool SDatabase::addLookupContainer(const std::string& name, std::unique_ptr<SLookupTable>&& lu)
{
    auto it = lu_containers.find(name);
    if (it != lu_containers.end()) return false;

    lu_containers.insert(std::make_pair(name, std::move(lu)));
    return true;
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
    auto it = lu_containers.find(name);
    if (it == lu_containers.end())
    {
        std::cerr << "Lookup table " << name << " not known!" << std::endl;
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    // TODO current id number
    long runid = 1e10;

    auto lu = it->second.get();

    // Check if Container was initialized from source, and if not, do it.
    auto it2 = conts_sources.find(name);
    if (it2 == conts_sources.end())
    {
        for (auto s : sources)
        {
            auto c = s->getContainer(name, runid);
            if (c)
            {
                conts_sources.emplace(name, s);
                lu->fromContainer(c);
                return lu;
            }
        }

        std::cerr << "Lookup table " << name << " could not be initialized from sources!"
                  << std::endl;
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    return lu;
}

/**
 * Add new calibration container.
 *
 * \param name container name
 * \param cal calibration object
 * \return success
 */
bool SDatabase::addCalContainer(const std::string& name,
                                std::unique_ptr<SVirtualCalContainer>&& cal)
{
    auto it = cal_containers.find(name);
    if (it != cal_containers.end()) return false;

    cal_containers.insert(std::make_pair(name, std::move(cal)));

    return true;
}

/**
 * Get calibration container by name.
 *
 * \param cont_name container name
 * \return pointer to container
 */
SVirtualCalContainer* SDatabase::getCalContainer(const std::string& name)
{
    // If SLookupTable was not registerd before, exit with failure.
    auto it = cal_containers.find(name);
    if (it == cal_containers.end())
    {
        std::cerr << "Calibration container " << name << " not known!" << std::endl;
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    // TODO current id number
    long runid = 1e10;

    auto cal = it->second.get();

    // Check if Container was initialized from source, and if not, do it.
    auto it2 = conts_sources.find(name);
    if (it2 == conts_sources.end())
    {
        for (auto s : sources)
        {
            auto c = s->getContainer(name, runid);
            if (c)
            {
                conts_sources.emplace(name, s);
                cal->fromContainer(c);
                return cal;
            }
        }

        std::cerr << "Calibration container " << name << " could not be initialized from sources!"
                  << std::endl;
        if (!sifi()->assertationsDisabled()) exit(EXIT_FAILURE);
        return nullptr;
    }

    return cal;
}
