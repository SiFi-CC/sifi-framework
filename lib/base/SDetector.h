// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDETECTOR_H
#define SDETECTOR_H

#include "SiFi.h"
#include "STaskManager.h"
#include "SParManager.h"

#include <TNamed.h>

class STask;

class SDetector : public TNamed
{
protected:
    // members
    UInt_t task_mask;       ///< mask on tasks

private:
    // constructor
    SDetector() {};

public:
    // constructor
    SDetector(const std::string & name);
    // destructor
    virtual ~SDetector() {};

    // methods
    /// Initialize tasks
    /// \return success
    virtual bool initTasks() = 0;
    /// Initialize containers
    /// \return success
    virtual bool initContainers() = 0;
    /// Initialize categories
    /// \return success
    virtual bool initCategories() = 0;

    void addTask(STask * task, Int_t step);

    /** Set tasks mask.
     * When the new task is add, it is logically conjugated with the mask, and
     * if the result is false, task is not add.
     *
     * \param m mask
     */
    void setTaskMask(UInt_t m) { task_mask = m; }
    /// Get tasks mask.
    /// \return tasks mask
    UInt_t getTaskMask() const { return task_mask; }

    /// Is it a simulation run
    /// \return simulation run flag
    bool isSimulation() const { return SiFi::instance()->isSimulation(); }

private:
    ClassDef(SDetector, 1);
};

#endif /* SDETECTOR_H */
