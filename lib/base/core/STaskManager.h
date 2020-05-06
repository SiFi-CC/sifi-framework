// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef STASKMANAGER_H
#define STASKMANAGER_H

#include "sifi_export.h"

#include <map>

class STask;

/**
 * This class manages alll the tasks when running the framework.
 * \todo Need more description.
 */
class SIFI_EXPORT STaskManager
{
protected:
    // members
    std::multimap<int, STask *> tasks;  ///< Tasks map
    static STaskManager * tm;           ///< Instance of STaskManager

private:
    // constructors
    STaskManager() = default;

public:
    /// Copy constructor
    STaskManager(STaskManager const &) = delete;
    // methods
    /// Assignment operator
    /// \return this object
    STaskManager & operator=(STaskManager const &) = delete;

    // instance method
    static STaskManager * instance();
    //destructor
    ~STaskManager();

    // methods
    void addTask(STask * task, int step);
    void initTasks();
    void reinitTasks();
    void runTasks();
    void finalizeTasks();
};

#endif /* STASKMANAGER_H */
