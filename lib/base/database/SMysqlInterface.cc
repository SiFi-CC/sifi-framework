// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SMysqlInterface.h"
#include "SCalContainer.h"
#include "SLookup.h"
#include "SPar.h"
#include "SParContainer.h"
#include "SParSource.h"
#include "SiFi.h"

#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <iostream>
#include <utility> // for pair, make_pair, move

/**
 * \class SMysqlInterface

\ingroup lib_base

*/

/**
 * Return run container for given runid.
 * \param runid run id
 * \return object or empty slot
 */
std::optional<SRunContainer> SMysqlInterface::getRunContainer(long runid) { return {}; }

/**
 * Return run containers from given run id range.
 * \param rinid_min minimal runid, weak ordering
 * \param runid_max maximal runid, strong ordering
 * \return array of objects, empty if no matches
 */
TObjArray SMysqlInterface::getRunContainers(long runid_min, long runid_max)
{
    TObjArray array;

    return array;
}

/**
 * Add new run contaniner. The runid is stored inside the container.
 * \param runcont run container to add.
 */
void SMysqlInterface::addRunContainer(SRunContainer&& runcont) {}

/**
 * Return named container for given runid.
 * \param name container name
 * \param runid run id
 * \return object or empty slot
 */
std::optional<SContainer> SMysqlInterface::getContainer(std::string_view&& name, long runid)
{
    return {};
}

/**
 * Return named containers from given run id range, from min until now/end.
 * \param name container name
 * \param rinid_min minimal runid, weak ordering
 * \return array of objects, empty if no matches
 */
std::vector<SContainer> SMysqlInterface::getContainers(std::string_view&& name, long runid_min)
{

    return {};
}

/**
 * Return named containers from given run id range.
 * \param name container name
 * \param rinid_min minimal runid, weak ordering
 * \param runid_max maximal runid, strong ordering
 * \return array of objects, empty if no matches
 */
std::vector<SContainer> SMysqlInterface::getContainers(std::string_view&& name, long runid_min,
                                                       long runid_max)
{
    return {};
}

/**
 * Add new container to the database, by name. It must be later validated via web interface.
 * \return success of the operation
 */
bool SMysqlInterface::addContainer(std::string_view&& name, SContainer&& cont) { return false; }
