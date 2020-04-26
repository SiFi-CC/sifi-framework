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

#include "SFibersStackGeomPar.h"
#include "SFibersStackCalibratorPar.h"
#include "SFibersStackDigitizerPar.h"

#include "SFibersStackUnpacker.h"
#include "SFibersStackCalibrator.h"
#include "SFibersStackDigitizer.h"

#include "SFibersStackDDUnpackerPar.h"

#include "SCalContainer.h"

#include "SiFi.h"

/** \class SFibersStackDetector
\ingroup lib_fibers_stack

An organizer class for the detector

\sa SDetector
*/

/** Constructor
 *
 * \param name detetcor name
 */
SFibersStackDetector::SFibersStackDetector(const std::string & name) : SDetector(name),
modules(1), layers(30), fibers(30)
{
}

/** Constructor
 *
 * \param name detetcor name
 * \param m number of modules
 * \param l number oflayers
 * \param f number of fibers
 */
SFibersStackDetector::SFibersStackDetector(const std::string & name, size_t m, size_t l, size_t f) :
    SDetector(name), modules(m), layers(l), fibers(f)
{
}

/** Destructor
 */
SFibersStackDetector::~SFibersStackDetector()
{
}

/** Init tasks
 *
 * \sa SPar::initTasks()
 * \return success
 */
bool SFibersStackDetector::initTasks()
{
    if (isSimulation())
    {
        addTask(new SFibersStackDigitizer(), 0);
    }
    else
    {
        addTask(new SFibersStackUnpacker(), 0);
        addTask(new SFibersStackCalibrator(), 1);
    }

    return true;
}

/** Init containers
 *
 * \sa SPar::initCategories()
 * \return success
 */
bool SFibersStackDetector::initContainers()
{
    pm()->addParameterContainer("SFibersStackGeomPar",  new SFibersStackGeomPar());

    if (isSimulation())
    {
        pm()->addParameterContainer("SFibersStackDigitizerPar",  new SFibersStackDigitizerPar());
    }
    else
    {
        pm()->addParameterContainer("SFibersStackDDUnpackerPar",  new SFibersStackDDUnpackerPar());
        pm()->addCalibrationContainer("SFibersStackCalibratorPar",
                                      new SFibersStackCalibratorPar("SFibersStackCalibratorPar"));
    }

    return true;
}

/** Init categries
 *
 * \sa SPar::initCategories()
 * \return success
 */
bool SFibersStackDetector::initCategories()
{
    size_t sizes[3];
    SiFi * dm = SiFi::instance();

    sizes[0] = modules;
    sizes[1] = layers;
    sizes[2] = fibers;
    if (isSimulation())
    {
        size_t sim_sizes[2];
        sim_sizes[0] = modules;
        sim_sizes[1] = layers * fibers;

        if (!dm->registerCategory(SCategory::CatGeantFibersRaw, "SGeantFibersRaw", 2, sim_sizes, true)) return false;
        if (!dm->registerCategory(SCategory::CatFibersStackCal, "SFibersStackCalSim", 3, sizes, true)) return false;
    }
    else
    {
        if (!dm->registerCategory(SCategory::CatFibersStackRaw, "SFibersStackRaw", 3, sizes, false)) return false;
        if (!dm->registerCategory(SCategory::CatFibersStackCal, "SFibersStackCal", 3, sizes, false)) return false;
    }

    return true;
}