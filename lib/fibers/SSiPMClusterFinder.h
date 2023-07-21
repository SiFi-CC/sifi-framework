// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/
 
#ifndef SSIPMCLUSTERFINDER_H
#define SSIPMCLUSTERFINDER_H

#include "STask.h"

#include <cstdio>

class SCategory;

class SSiPMClusterFinder : public STask
{
    
protected:
    SCategory* catSiPMsHit{nullptr};      ///< category containing SiPMs hits
    SCategory* catSiPMsCluster{nullptr};  ///< category containing SiPMs clusters
    
public:
    /// Default constructor
    SSiPMClusterFinder() = default;
    /// Default destructor
    virtual ~SSiPMClusterFinder() = default;
    
    /// Initialize task
    /// \sa STask::init()
    /// \return success
    bool init() override;
    
    /// Execute task
    /// \sa STask::execute()
    /// \return success
    bool execute() override;
    
    /// Finalize task.
    /// \sa STask::finalize()
    /// \return success
    bool finalize() override;
    
    void print();
    
};

#endif /* SSIPMCLUSTERFINDER_H */
