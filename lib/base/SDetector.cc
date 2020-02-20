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
#include "STask.h"
#include "STaskManager.h"

/** \class SDetector
\ingroup lib_base

An interface class for detector description

Mask on task is interpreted as a bit mask. When task with step n is add, the
n-bit of the mask is tested andif false, then task is ignored. It puts constrain
on maximal number of steps, but 32 steps seems sufficient for most of the cases.
*/

/**
 * Constructor
 * \param name detector name
 */
SDetector::SDetector(const std::string & name) : TNamed(name.c_str(), name.c_str()), task_mask(0xff)
{
}

/**
 * Add task to the detector at given task step. Task are executed starting from
 * the lowest to the highest step for all the detectors. Tasks at the same level
 * can be executed in undetermined order.
 *
 * \param task task to execute
 * \param step task step value
 */
void SDetector::addTask(STask * task, Int_t step)
{
    // add task only is mask allows for it
    if (task_mask & (1 << step))
        STaskManager::instance()->addTask(task, step);
}

ClassImp(SDetector);
