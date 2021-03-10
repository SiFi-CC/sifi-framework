// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDETECTORMANAGER_H
#define SDETECTORMANAGER_H

#include "sifi_export.h"

#include <vector>

class SDetector;

class SIFI_EXPORT SDetectorManager
{
  private:
    // members
    std::map<std::string, SDetector*> detectors; ///< map det name to object
    static SDetectorManager* detm;               ///< Instance of the SDetectorManager

  private:
    // constructors
    SDetectorManager() = default;

  public:
    SDetectorManager(SDetectorManager const&) = delete;
    SDetectorManager& operator=(SDetectorManager const&) = delete;

    // instance method
    static SDetectorManager* instance();
    // destructor
    virtual ~SDetectorManager();

    // methods
    void addDetector(SDetector* detector);
    SDetector* getDetector(const std::string& name);

    void initTasks();
    void initParameterContainers();
    void initCategories();
};

extern SDetectorManager* dm();

#endif /* STASKMANAGER_H */
