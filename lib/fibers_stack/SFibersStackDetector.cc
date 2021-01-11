// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackDetector.h"

#include "SCalContainer.h"

#include "SFibersStackCalibratorPar.h"
#include "SFibersStackDigitizerPar.h"
#include "SFibersStackGeomPar.h"

#include "SFibersStackCalibrator.h"
#include "SFibersStackDigitizer.h"
#include "SFibersStackUnpacker.h"

#include "SFibersStackDDUnpackerPar.h"
#include "SFibersStackHitFinder.h"
#include "SFibersStackHitFinderPar.h"

#include "SCalContainer.h"

#include "SiFi.h"

/**
 * \class SFibersStackDetector
\ingroup lib_fibers_stack

An organizer class for the detector

\sa SDetector
*/

/**
 * Constructor
 *
 * \param name detetcor name
 */
SFibersStackDetector::SFibersStackDetector(const std::string& name)
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
SFibersStackDetector::SFibersStackDetector(const std::string& name, size_t m, size_t l, size_t f)
    : SDetector(name), modules(m), layers(l), fibers(f)
{
}

/**
 * Init tasks
 *
 * \sa SPar::initTasks()
 * \return success
 */
bool SFibersStackDetector::initTasks()
{
    if (isSimulation())
    {
        //         addTask(new SFibersStackDigitizer(), 0); FIXME collides with DR Importer
        addTask(new SFibersStackHitFinder(), 2);
    }
    else
    {
        addTask(new SFibersStackUnpacker(), 0);
        addTask(new SFibersStackCalibrator(), 1);
        addTask(new SFibersStackHitFinder(), 2);
    }

    return true;
}

/**
 * Init containers
 *
 * \sa SPar::initCategories()
 * \return success
 */
bool SFibersStackDetector::initContainers()
{
    pm()->addParameterContainer("SFibersStackGeomPar", new SFibersStackGeomPar());

    if (isSimulation())
    {
        pm()->addParameterContainer("SFibersStackDigitizerPar", new SFibersStackDigitizerPar());
    }
    else
    {
        pm()->addParameterContainer("SFibersStackDDUnpackerPar", new SFibersStackDDUnpackerPar());
        pm()->addCalibrationContainer("SFibersStackCalibratorPar",
                                      new SFibersStackCalibratorPar("SFibersStackCalibratorPar"));
    }

    pm()->addCalibrationContainer("SFibersStackHitFinderFiberPar",
                                  new SCalContainer<2>("SFibersStackHitFinderFiberPar"));
    pm()->addParameterContainer("SFibersStackHitFinderPar", new SFibersStackHitFinderPar());
    return true;
}

/**
 * Init categries
 *
 * \sa SPar::initCategories()
 * \return success
 */
bool SFibersStackDetector::initCategories()
{
    size_t sizes[3];
    SiFi* dm = SiFi::instance();

    sizes[0] = modules;
    sizes[1] = layers;
    sizes[2] = fibers;
    if (isSimulation())
    {
        if (!dm->registerCategory(SCategory::CatGeantFibersRaw, "SGeantFibersRaw", 1000, true))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersStackCal, "SFibersStackCalSim", 3, sizes,
                                  true))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersStackHit, "SFibersStackHit", 3, sizes, true))
            return false;
    }
    else
    {
        if (!dm->registerCategory(SCategory::CatFibersStackRaw, "SFibersStackRaw", 3, sizes, false))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersStackCal, "SFibersStackCal", 3, sizes, false))
            return false;
        if (!dm->registerCategory(SCategory::CatFibersStackHit, "SFibersStackHit", 3, sizes, false))
            return false;
    }

    if (!dm->registerCategory(SCategory::CatFibersStackClus, "SFibersStackClus", 10, false))
        return false;

    return true;
}
