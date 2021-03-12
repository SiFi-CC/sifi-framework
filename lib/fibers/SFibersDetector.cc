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

#include "SFibersCalibratorPar.h"
#include "SFibersDDCalibratorPar.h"
#include "SFibersDigitizerPar.h"
#include "SFibersGeomPar.h"

#include "SFibersCalibrator.h"
#include "SFibersDigitizer.h"
#include "SFibersUnpacker.h"

#include "SFibersDDUnpackerPar.h"
#include "SFibersHitFinder.h"
#include "SFibersHitFinderPar.h"

#include "SFibersClusterFinder.h"
#include "SFibersClusterFinderPar.h"

#include "SCalContainer.h"

#include "SiFi.h"

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
    pm()->addParameterContainer("FibersGeomPar", new SFibersGeomPar());

    if (isSimulation())
    {
        pm()->addParameterContainer("FibersDigitizerPar", new SFibersDigitizerPar());
    }
    else
    {
        pm()->addParameterContainer("FibersDDUnpackerPar", new SFibersDDUnpackerPar());
        pm()->addCalibrationContainer(
            "FibersDDCalibratorPar",
            new SFibersDDCalibratorPar("FibersDDCalibratorPar"));
        pm()->addCalibrationContainer("FibersCalibratorPar",
                                      new SFibersCalibratorPar("FibersCalibratorPar"));
    }

    pm()->addCalibrationContainer("FibersHitFinderFiberPar",
                                  new SCalContainer<3>("FibersHitFinderFiberPar"));
    pm()->addParameterContainer("FibersHitFinderPar", new SFibersHitFinderPar());

    pm()->addParameterContainer("FibersClusterFinderPar", new SFibersClusterFinderPar());

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
        if (!dm->registerCategory(SCategory::CatFibersCal, "SFibersCalSim", 3, sizes,
                                  true))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersHit, "SFibersHitSim", 3, sizes,
                                  true))
            return false;
    }
    else
    {
        if (!dm->registerCategory(SCategory::CatFibersRaw, "SFibersRaw", 3, sizes, false))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersCal, "SFibersCal", 3, sizes, false))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersHit, "SFibersHit", 3, sizes, false))
            return false;
    }

    if (!dm->registerCategory(SCategory::CatFibersClus, "SFibersCluster", 2, sizes_clus,
                              false))
        return false;

    return true;
}
