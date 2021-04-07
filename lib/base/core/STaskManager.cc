// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "STaskManager.h"
#include "STask.h"

#include <cstdlib>
#include <iostream>
#include <utility>

/**
 * \class SDatabase
\ingroup lib_base

Task Manager class. Manages all tasks.

It's a singleton class of which object can be obtained using instance() method.

Tasks mamager must be initializatied before MDetectorManager::initTasks()
is called since it adds detector tasks to the manager.

Each tasks is add with given step value. The tasks are executed from step 0
until the last registered step. Tasks at the same step are not guaraneteed to
always execute in the same order. Add tasks at diferent steps to preserve
execution order.
*/

STaskManager* STaskManager::tm = nullptr;

/**
 * Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
STaskManager* STaskManager::instance()
{
    if (!tm) tm = new STaskManager;

    return tm;
}

/**
 * Default destructor
 */
STaskManager::~STaskManager()
{
    for (auto& t : tasks)
        delete t.second;
}

/**
 * Add task at given step
 *
 * \param task pointer to task object
 * \param step step value
 */
void STaskManager::addTask(STask* task, int step)
{
    tasks.insert(std::pair<int, STask*>(step, task));
}

/**
 * Init all tasks
 */
void STaskManager::initTasks()
{
    for (const auto& t : tasks)
    {
        bool res = t.second->init();

        if (!res)
        {
            std::cerr << "Init failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

/**
 * Reinitialize all tasks
 */
void STaskManager::reinitTasks()
{
    for (const auto& t : tasks)
    {
        bool res = t.second->reinit();

        if (!res)
        {
            std::cerr << "Reinit failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

/**
 * Run tasks.
 *
 * Start with step 0 until the last step.
 */
void STaskManager::runTasks()
{
    for (const auto& t : tasks)
    {
        bool res = t.second->execute();

        if (!res)
        {
            std::cerr << "Task failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

/**
 * Finalize tasks
 */
void STaskManager::finalizeTasks()
{
    for (const auto& t : tasks)
    {
        bool res = t.second->finalize();

        if (!res)
        {
            std::cerr << "Finalize failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}
