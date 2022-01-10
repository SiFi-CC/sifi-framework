// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SDetector.h"

#include "SDetectorManager.h"

#include <spdlog/spdlog.h>

#include <cstdlib> // for exit, EXIT_FAILURE
#include <iostream>
#include <utility> // for pair

/**
 * \class SDetectorManager
\ingroup lib_base

Detector Manager class.

It's a singleton class of which object can be obtained using instance() method.

Each used detector must be registered here using addDetector() method.

After all detectors are add, one can initialzie tasks for all detectors,
parameters, and containers, using following methods:
initTasks(), initParameterContainers(), initCategories().

Particularly, the Parameter Containers must be initialized after the Parameter
Manager MParameterManager as initParameterContainers() checks that all
required parameter containers exists.

*/

SDetectorManager* SDetectorManager::detm = nullptr;

/**
 * Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SDetectorManager* SDetectorManager::instance()
{
    if (!detm) detm = new SDetectorManager;

    return detm;
}

/**
 * Shortcut
 * \return SDetectorManager instance
 */
SDetectorManager* dm() { return SDetectorManager::instance(); }

SDetectorManager::~SDetectorManager()
{
    for (auto& d : detectors)
        if (d.second) delete d.second;
}

/**
 * Add detector.
 *
 * \param detector pointer to a detector object
 */
void SDetectorManager::addDetector(SDetector* detector)
{
    detectors.insert(std::pair<std::string, SDetector*>(detector->GetName(), detector));
    spdlog::info("[{0}] Detector {1} added.", __func__, detector->GetName());

    auto res = detector->initTasks();
    if (!res)
    {
        spdlog::error("[{0}] Tasks init failed.", __func__);
        std::exit(EXIT_FAILURE);
    }

    res = detector->initContainers();
    if (!res)
    {
        spdlog::error("[{0}] Tasks init failed.", __func__);
        std::exit(EXIT_FAILURE);
    }

    res = detector->initCategories();
    if (!res)
    {
        spdlog::error("[{0}] Categories init failed.", __func__);
        std::exit(EXIT_FAILURE);
    }
}

/**
 * Get detector object by its name.
 *
 * \param name detector name
 * \return pointer to the detector object
 */
SDetector* SDetectorManager::getDetector(const std::string& name)
{
    std::map<std::string, SDetector*>::iterator it = detectors.find(name);
    if (it != detectors.end())
        return it->second;
    else
        return nullptr;
}
