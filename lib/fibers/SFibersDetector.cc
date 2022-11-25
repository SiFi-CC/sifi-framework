// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersDetector.h"

#include "SCalContainer.h"
#include "SCategory.h" // for SCategory, SCategory::CatFibersCal
#include "SDatabase.h" // for pm, SDatabase
#include "SFibersCalibrator.h"
#include "SFibersCalibratorPar.h"
#include "SFibersClusterFinder.h"
#include "SFibersClusterFinderPar.h"
#include "SFibersDDCalibratorPar.h"
#include "SFibersDDUnpackerPar.h"
#include "SFibersDigitizerPar.h"
#include "SFibersGeomPar.h"
#include "SFibersHitFinder.h"
#include "SFibersHitFinderPar.h"
#include "SFibersUnpacker.h"
#include "SPar.h" // for SPar
#include "SiFi.h"

#include <memory> // for make_unique, allocator

/**
 * \class SFibersDetector
\ingroup lib_fibers

An organizer class for the detector

\sa SDetector
*/

/**
 * Constructor
 *
 * \param name detetcor name
 */
SFibersDetector::SFibersDetector(const std::string& name)
    : SDetector(name), modules(2), layers(30), fibers(76)
{
}

/**
 * Constructor
 *
 * \param name detetcor name
 * \param m number of modules
 * \param l number oflayers
 * \param f number of fibers
 */
SFibersDetector::SFibersDetector(const std::string& name, size_t m, size_t l, size_t f)
    : SDetector(name), modules(m), layers(l), fibers(f)
{
}

/**
 * Init tasks
 *
 * \sa SPar::initTasks()
 * \return success
 */
bool SFibersDetector::initTasks()
{
    if (isSimulation())
    {
        //         addTask(new SFibersDigitizer(), 0); FIXME collides with DR Importer
        addTask(new SFibersHitFinder(), 2);
        addTask(new SFibersClusterFinder(), 3);
    }
    else
    {
        addTask(new SFibersUnpacker(), 0);
        addTask(new SFibersCalibrator(), 1);
        addTask(new SFibersHitFinder(), 2);
        addTask(new SFibersClusterFinder(), 3);
    }

    return true;
}

/**
 * Init containers
 *
 * \sa SPar::initCategories()
 * \return success
 */
bool SFibersDetector::initContainers()
{
    pm()->addParContainer("FibersGeomPar", std::make_unique<SFibersGeomPar>());

    if (isSimulation())
    {
        pm()->addParContainer("FibersDigitizerPar", std::make_unique<SFibersDigitizerPar>());
    }
    else
    {
        pm()->addParContainer("FibersDDUnpackerPar", std::make_unique<SFibersDDUnpackerPar>());
        pm()->addCalContainer("FibersDDCalibratorPar",
                              std::make_unique<SFibersDDCalibratorPar>("FibersDDCalibratorPar"));
        pm()->addCalContainer("FibersCalibratorPar",
                              std::make_unique<SFibersCalibratorPar>("FibersCalibratorPar"));
    }

    pm()->addCalContainer("FibersHitFinderFiberPar",
                          std::make_unique<SCalContainer<3>>("FibersHitFinderFiberPar"));
    pm()->addParContainer("FibersHitFinderPar", std::make_unique<SFibersHitFinderPar>());

    pm()->addParContainer("FibersClusterFinderPar", std::make_unique<SFibersClusterFinderPar>());

    return true;
}

/**
 * Init categries
 *
 * \sa SPar::initCategories()
 * \return success
 */
bool SFibersDetector::initCategories()
{
    size_t sizes[3];
    SiFi* dm = SiFi::instance();

    sizes[0] = modules;
    sizes[1] = layers;
    sizes[2] = fibers;

    size_t sizes_clus[2];
    sizes_clus[0] = modules;
    sizes_clus[1] = 10;

    if (isSimulation())
    {
        if (!dm->registerCategory(SCategory::CatGeantFibersRaw, "SGeantFibersRaw", 250, true))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersCal, "SFibersCalSim", 3, sizes, true))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersHit, "SFibersHitSim", 3, sizes, true))
            return false;
    }
    else
    {
        if (!dm->registerCategory(SCategory::CatFibersRaw, "SFibersRaw", 3, sizes, false))
            return false;
//         if (!dm->registerCategory(SCategory::CatSiPMHit, "SSiPMHit", 3, sizes, false))
//             return false;
        if (!dm->registerCategory(SCategory::CatFibersCal, "SFibersCal", 3, sizes, false))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersHit, "SFibersHit", 3, sizes, false))
            return false;
    }

    if (!dm->registerCategory(SCategory::CatFibersClus, "SFibersCluster", 2, sizes_clus, false))
        return false;

    return true;
}
