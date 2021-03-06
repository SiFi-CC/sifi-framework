// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParDatabaseSource.h"

#include "SContainer.h"
#include "SDatabase.h"

#include "tabulate/cell.hpp"           // for Cell
#include "tabulate/color.hpp"          // for Color, Color::red, Color::yellow
#include "tabulate/column.hpp"         // for Column, ColumnFormat::font_align
#include "tabulate/column_format.hpp"  // for ColumnFormat
#include "tabulate/font_align.hpp"     // for FontAlign, FontAlign::center
#include "tabulate/font_style.hpp"     // for FontStyle, FontStyle::bold
#include "tabulate/format.hpp"         // for Format
#include "tabulate/row.hpp"            // for Row
#include "tabulate/table_internal.hpp" // for Cell::format

#include <ctime>
#include <cxxabi.h> // for __forced_unwind
#include <iomanip>
#include <iostream>
#include <string_view>
#include <utility> // for pair
#include <variant> // for variant

/**
 * \class SParDatabaseSource

\ingroup lib_base_database

Reads parameters from the database file. Parameters in the database are organized in:
* releases - collections of paramaters related to given experiment, test, measurement
* versions (ranges) - specific value of parameters valid in specific time range
* runid - run number (from DAQ)

The database interface must provide following interface:

 1. Getting data from database:

   - Read parameater values for given time slice

     `getContainer(release : string, container_name : string, runid : long) -> SContainer`

   - Read all paramater ranges for given release

     `getContainer(release : string, container_name : string) -> std::vector<SContainer>`

   - Read selected paramater ranges for given release

     `getContainer(release : string, range : range) -> std::vector<SContainer>`

 2. Writing data from database:

   - Write slice (ascii) to database as range. Returns 0 if succeed, otherwise positive value as
     error code (e.g. 1 - overlap, etc.)

     `writeContainer(release : string, container_name : string, range : range, container :
SContainer) -> int`

*/

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
SParDatabaseSource::SParDatabaseSource() : SParSource()
{
    mysqlcon = std::make_unique<SMysqlInterface>("http://localhost", 1234);
    parseSource();
}

/**
 * Parse source file. Implemented based on hadd.C from ROOT.
 *
 * \return success
 */
bool SParDatabaseSource::parseSource() { return true; }

SContainer* SParDatabaseSource::getContainer(const std::string& name, long runid)
{
    // check if same release
    std::string_view release = SDatabase::instance()->getRelease();
    // TODO if release has name, then check whether it matches the one from file
    // if (!release.empty() and release != this_release_from_file) return 0;

    // DB call
    // DBOBJECT->getContainer(release, name, runid);
    mysqlcon->setParamRelease(release);
    mysqlcon->getContainer(name, runid);

    // check if container is in the source at all
    auto it = containers.find(name);
    if (it == containers.end()) { return nullptr; }

    // if it was the same version like before, return cached one
    SContainer* last = last_container[name];
    if (last and last->validity == runid) return last;

    // get fresh version, need to set flag reinit! TODO
    // also runid -> time conversion
    auto time = runid;

    auto&& cont_map = containers[name];
    auto it2 = cont_map.lower_bound(validity_range_t(time, time));
    if (it2 != cont_map.end())
    {
        if (it2->second->validity == time)
        {
            // TODO force DB to reinit here
            return it2->second;
        }
    }

    return nullptr;
}

bool SParDatabaseSource::setContainer(const std::string& name, SContainer&& cont)
{
    // check if same release
    std::string_view release = SDatabase::instance()->getRelease();
    // TODO if release has name, then check whether it matches the one from file
    // if (!release.empty() and release != this_release_from_file) return 0;

    // DB call
    // DBOBJECT->getContainer(release, name, runid);
    mysqlcon->setParamRelease(release);
    return mysqlcon->addContainer(std::move(name), std::move(cont));
}

#include "tabulate/table.hpp"
using namespace tabulate;

void SParDatabaseSource::print() const
{
    std::cout << "=== Daatbase Source Info ===" << std::endl;
    std::cout << "    Database: " << source << std::endl;

    for (auto& container : containers)
    {
        const validity_range_t* cache = nullptr;
        Table cont_summary;
        cont_summary.add_row({container.first, "Valid from", "Valid to", "Overlap", "Truncated"});

        for (auto& revision : container.second)
        {
            std::stringstream s_from;
            s_from << std::put_time(std::gmtime(&revision.first.from), "%c %Z");

            std::stringstream s_to;
            s_to << std::put_time(std::gmtime(&revision.first.to), "%c %Z");

            std::stringstream trunc_from;
            if (revision.first.truncated > 0)
                trunc_from << std::put_time(std::gmtime(&revision.first.truncated), "%c %Z");

            bool overlap = cache ? revision.first.check_overlap(*cache) : false;

            cont_summary.add_row(
                {"", s_from.str(), s_to.str(), std::to_string(overlap), trunc_from.str()});

            cache = &revision.first;
        }

        cont_summary.column(3).format().font_align(FontAlign::center);
        cont_summary.column(4).format().font_align(FontAlign::center).font_color(Color::red);

        for (size_t i = 0; i < 5; ++i)
        {
            cont_summary[0][i]
                .format()
                .font_color(Color::yellow)
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold});
        }

        std::cout << cont_summary << std::endl;
    }
}
