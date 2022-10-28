// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SEventBuilderConnector.h"

SEventBuilderConnector::SEventBuilderConnector()
{
    target = std::make_unique<SParDatabaseSource>();
}

auto SEventBuilderConnector::openRun(int run_type, std::string file_name) -> bool
{
    return openRun(std::chrono::system_clock::now(), run_type, file_name);
}

auto SEventBuilderConnector::openRun(std::chrono::time_point<std::chrono::system_clock> ts,
                                     int run_type, std::string file_name) -> bool
{
    auto run =
        target->openRunContainer(run_type, std::chrono::system_clock::to_time_t(ts), file_name);
    return run.has_value();
}

auto SEventBuilderConnector::closeRun() -> bool
{
    return closeRun(std::chrono::system_clock::now());
}

auto SEventBuilderConnector::closeRun(std::chrono::time_point<std::chrono::system_clock> ts) -> bool
{
    auto run = target->closeRunContainer(std::chrono::system_clock::to_time_t(ts));
    return run.has_value();
}
