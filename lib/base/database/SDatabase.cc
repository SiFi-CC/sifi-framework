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
    if (it != obj_factory.end()) return false;

    obj_factory.insert(std::make_pair(name, std::move(f)));
    //     container_mode[name] = ContainerMode::Read;

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
        std::cerr << "Parameter container " << name << " not known!" << std::endl;
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
        std::cerr << "Lookup table " << name << " not known!" << std::endl;
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
        std::cerr << "Calibration container " << name << " not known!" << std::endl;
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

    return cal;
}
