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

#include <map>

#include "SiFiManager.h"

class STask;

class STaskManager
{
protected:
    // members
    typedef std::multimap<int, STask *> TasksMap;   ///< TaskMap type
    TasksMap tasks;                 ///< Tasks map
    static STaskManager * tm;       ///< Instance of the STaskManager

private:
    // constructors
    STaskManager();
    /// Copy constructor
    STaskManager(STaskManager const &) {}
    // methods
    /// Assignment operator
    /// \return this object
    STaskManager & operator=(STaskManager const &) { return *this; }

public:
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
