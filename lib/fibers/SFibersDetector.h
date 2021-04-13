// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSDETECTOR_H
#define SFIBERSDETECTOR_H

#include "sifi_export.h"

#include "SDetector.h"

#include <cstddef> // for size_t
#include <string>  // for string

class SIFI_EXPORT SFibersDetector : public SDetector
{
protected:
    // members
    const size_t modules; ///< number of modules
    const size_t layers;  ///< number of layers
    const size_t fibers;  ///< number of fibers

public:
    // constructors
    SFibersDetector() = delete;
    explicit SFibersDetector(const std::string& name);
    SFibersDetector(const std::string& name, size_t m, size_t l, size_t f);
    // destructor
    ~SFibersDetector() = default;

    // methods
    bool initTasks() override;
    bool initContainers() override;
    bool initCategories() override;
};

#endif /* SFIBERSDETECTOR_H */
