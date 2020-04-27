// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include "SDetector.h"

#include "SDetectorManager.h"

/** \class SDetectorManager
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

SDetectorManager * SDetectorManager::detm = nullptr;

/**
 * Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SDetectorManager * SDetectorManager::instance()
{
    if (!detm)
        detm = new SDetectorManager;

    return detm;
}

/** Shortcut
 * \return SDetectorManager instance
 */
SDetectorManager * dm()
{
    return SDetectorManager::instance();
}

/**
 * Add detector.
 *
 * \param detector pointer to a detector object
 */
void SDetectorManager::addDetector(SDetector* detector)
{
    detectors.insert(std::pair<std::string, SDetector*>(detector->GetName(), detector));
    printf("Detector %s add\n", detector->GetName());
}

/**
 * Get detector object by its name.
 *
 * \param name detector name
 * \return pointer to the detector object
 */
SDetector * SDetectorManager::getDetector(const std::string& name)
{
    DetectorsMap::iterator it = detectors.find(name);
    if (it != detectors.end())
        return it->second;
    else
        return nullptr;
}

/**
 * Init all detectors tasks.
 */
void SDetectorManager::initTasks()
{
    for (DetectorsMap::iterator it = detectors.begin(); it != detectors.end(); ++it)
    {
        bool res = it->second->initTasks();

        if (!res)
        {
            std::cerr << "Tasks init failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

/**
 * Init all detectors parameter containers.
 * 
 * It must be called after the MParameterManager has been setup.
 */
void SDetectorManager::initParameterContainers()
{
    for (DetectorsMap::iterator it = detectors.begin(); it != detectors.end(); ++it)
    {
        bool res = it->second->initContainers();

        if (!res)
        {
            std::cerr << "Tasks init failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

/**
 * Init all detectors categories.
 * 
 * It must be called before any detector categor is opened or created. See
 * SiFiManager and MCategory for details about categories.
 */
void SDetectorManager::initCategories()
{
    for (DetectorsMap::iterator it = detectors.begin(); it != detectors.end(); ++it)
    {
        bool res = it->second->initCategories();

        if (!res)
        {
            std::cerr << "Categories init failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}
