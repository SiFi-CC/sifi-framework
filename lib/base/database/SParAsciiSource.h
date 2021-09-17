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

#ifndef SPARASCIISOURCE_H
#define SPARASCIISOURCE_H

#include "sifi_export.h"

#include "SParSource.h"

#include <map>
#include <memory>
#include <string>

class SContainer;

class SIFI_EXPORT SParAsciiSource : public SParSource
{
public:
    SParAsciiSource(const std::string& source);
    SParAsciiSource(std::string&& source);
    virtual ~SParAsciiSource() = default;

    virtual auto setOpenMode(SourceOpenMode mode) -> void override {}

    virtual auto findContainer(const std::string& name) -> bool override;

    virtual auto getContainer(const std::string& name, long runid)
        -> std::shared_ptr<SContainer> override;

    void print() const override;

private:
    bool parseSource();

private:
    std::string source;                                            ///< ascii file name
    std::map<std::string, std::shared_ptr<SContainer>> containers; ///< Containers mirrors
};

#endif /* SPARASCIISOURCE_H */
