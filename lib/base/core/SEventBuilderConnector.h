// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SEVENTBUILDERCONNECTOR_H
#define SEVENTBUILDERCONNECTOR_H

#include "sifi_export.h"

#include "SParDatabaseSource.h"

#include <chrono>
#include <memory>
#include <string>

class SParDatabaseSource;

class SIFI_EXPORT SEventBuilderConnector
{
public:
    SEventBuilderConnector();

    auto openRun(int run_type, std::string file_name) -> bool;
    auto openRun(std::chrono::time_point<std::chrono::system_clock> ts, int run_type,
                 std::string file_name) -> bool;

    auto closeRun() -> bool;
    auto closeRun(std::chrono::time_point<std::chrono::system_clock> ts) -> bool;

private:
    std::unique_ptr<SParDatabaseSource> target;
};

#endif /* STASKMANAGER_H */
