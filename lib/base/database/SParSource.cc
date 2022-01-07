// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParSource.h"

#include "SRun.h"

#include <iostream>

/**
 * \class SParSource

\ingroup lib_base

Parameters Manager class. Stores and dumps all parameters used by the framework.

It's a singleton class of which object can be obtained using instance() method.

Paramaters mamager must be initializatied before
MDetectorManager::initParameterContainers() is called since it checks whether
the requested parameter containers exists.
*/

// for trim functions see
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

auto SParSource::setOpenMode(SourceOpenMode mode) -> void { doSetOpenMode(mode); }

auto SParSource::findContainer(const std::string& name) -> bool { return doFindContainer(name); }

auto SParSource::getContainer(const std::string& name, ulong runid) -> std::shared_ptr<SContainer>
{
    return doGetContainer(name, runid);
}

auto SParSource::insertContainer(const std::string& name, SContainer* cont) -> bool
{
    return doInsertContainer(name, cont);
}

auto SParSource::insertContainer(const std::string& name, const std::vector<SContainer*>& cont)
    -> bool
{
    return doInsertContainer(name, cont);
}

auto SParSource::getRuns() -> std::vector<SRun> { return doGetRuns(); }

auto SParSource::getRun(ulong runid) -> SRun { return doGetRun(runid); }

auto SParSource::insertRun(SRun run) -> bool { return doInsertRun(run); }

auto SParSource::getExperiment() const -> std::optional<SExperiment> { return doGetExperiment(); }

auto SParSource::print() const -> void { doPrint(); }
