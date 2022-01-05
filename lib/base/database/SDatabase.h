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

#include "SContainer.h"
#include "SRun.h"
// #include "SLookup.h"

#include <algorithm> // for max
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

class SPar;
class SParSource;
class SParContainer;
class SVirtualCalContainer;
class SLookupTable;

using cont_obj = std::variant<SPar*, SVirtualCalContainer*, SLookupTable*>;
using cont_obj_factory = std::function<cont_obj()>;

class SIFI_EXPORT SDatabase
{
protected:
    std::vector<SParSource*> sources; ///< Parameters source file
    SParSource* target{0};            ///< Parameters destination file

    std::map<std::string, SParSource*> conts_sources;                            ///< Input sources

    static SDatabase* pm; ///< Instance of the SDatabase
    std::map<std::string, cont_obj_factory> obj_factory;       ///< Container object factory

    std::string release; ///< stores parameters release name
    unsigned long current_runid;
    unsigned long reference_runid{0};

public:
    SDatabase();
    ~SDatabase();

    // methods
    /// Set parameters source
    /// \param source source file name
    void addSource(SParSource* source);
    /// Set parameters destination
    /// \param target destination file name
    void setTarget(SParSource* target);

    void writeDestination();
    void writeContainers(const std::vector<std::string>& names);

    bool addContainer(const std::string& name, cont_obj_factory&& f);
    SPar* getParContainer(const std::string& name);
    SLookupTable* getLookupContainer(const std::string& name);
    SVirtualCalContainer* getCalContainer(const std::string& name);



    /// Set release value.
    /// \param r release name
    void setRelease(std::string_view r) { release = r; }
    /// Get release value.
    /// \return release value
    auto getRelease() const { return release; }

    void print() const;

    friend class SParAsciiSource;
};

/**
 * Helper class to provide unique interface for db whichever class is used. Could be as well
 * a namespace but then the mdb variable would be exposed.
 */
class SIFI_EXPORT SRuntimeDb
{
private:
    // constructors
    SRuntimeDb() = default;
    SRuntimeDb(SRuntimeDb const&) = delete;

    SRuntimeDb& operator=(SRuntimeDb const&) = delete;

    static SDatabase* mdb; ///< Instance of the SDatabase

public:
    static void init(SDatabase* db) { mdb = db; }
    static auto get() -> SDatabase* { return mdb; };
};

extern SIFI_EXPORT SDatabase* rdb();

#endif /* SDATABASE_H */
