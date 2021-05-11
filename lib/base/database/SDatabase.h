/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDATABASE_H
#define SDATABASE_H

#include "sifi_export.h"

#include <algorithm> // for max
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class SPar;
class SParSource;
class SParContainer;
class SVirtualCalContainer;
class SLookupTable;

class SIFI_EXPORT SDatabase
{
protected:
    std::vector<SParSource*> sources; ///< Parameters source file
    SParSource* target{0};            ///< Parameters destination file

    std::map<std::string, SParSource*> conts_sources;                            ///< Input sources
    std::map<std::string, std::unique_ptr<SParContainer>> par_containers;        ///< Par Containers
    std::map<std::string, std::unique_ptr<SPar>> par_parameters;                 ///< Parameters
    std::map<std::string, std::unique_ptr<SVirtualCalContainer>> cal_containers; ///< Par Containers
    std::map<std::string, std::unique_ptr<SLookupTable>> lu_containers; ///< Lookup Containers

    static SDatabase* pm; ///< Instance of the SDatabase

    std::string_view release; ///< stores parameters release name

private:
    // constructors
    SDatabase() = default;
    SDatabase(SDatabase const&) = delete;

    SDatabase& operator=(SDatabase const&) = delete;

public:
    // instance method
    static SDatabase* instance();
    // destructor
    ~SDatabase();

    // methods
    /// Set parameters source
    /// \param source source file name
    void addSource(SParSource* source) { sources.push_back(source); }
    /// Set parameters destination
    /// \param target destination file name
    void setTarget(SParSource* target) { this->target = target; }

    void writeDestination();
    void writeContainers(const std::vector<std::string>& names);

    bool addParContainer(const std::string& name, std::unique_ptr<SPar>&& par);
    SPar* getParContainer(const std::string& name);

    bool addLookupContainer(const std::string& name, std::unique_ptr<SLookupTable>&& lu);
    SLookupTable* getLookupContainer(const std::string& name);

    bool addCalContainer(const std::string& name, std::unique_ptr<SVirtualCalContainer>&& cal);
    SVirtualCalContainer* getCalContainer(const std::string& name);

    /// Set release value.
    /// \param r release name
    void setRelease(std::string_view r) { release = r; }
    /// Get release value.
    /// \return release value
    std::string_view getRelease() const { return release; }

    void print() const;

    friend class SParAsciiSource;
};

extern SIFI_EXPORT SDatabase* pm();

#endif /* SDATABASE_H */
