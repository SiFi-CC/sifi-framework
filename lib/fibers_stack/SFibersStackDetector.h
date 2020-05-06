// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKDETECTOR_H
#define SFIBERSSTACKDETECTOR_H

#include "sifi_export.h"

#include "SDetector.h"

class SIFI_EXPORT SFibersStackDetector : public SDetector
{
protected:
    // members
    const size_t modules;   ///< number of modules
    const size_t layers;    ///< number of layers
    const size_t fibers;    ///< number of fibers

public:
    // constructors
    SFibersStackDetector() = delete;
    explicit SFibersStackDetector(const std::string & name);
    SFibersStackDetector(const std::string & name, size_t m, size_t l, size_t f);
    // destructor
    ~SFibersStackDetector() = default;

    // methods
    bool initTasks() override;
    bool initContainers() override;
    bool initCategories() override;
};

#endif /* SFIBERSSTACKDETECTOR_H */
