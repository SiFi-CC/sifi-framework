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

#ifndef SPARMANAGER_H
#define SPARMANAGER_H

#include "sifi_export.h"

#include <iostream>
#include <map>
#include <vector>

class SPar;
class SParContainer;
class SVirtualCalContainer;
class SLookupTable;

void trim(std::string& s);
void simplify(std::string& s);
bool isFloat(const std::string& str);

/**
 * Stores content of the container read out from the disk.
 */
struct SIFI_EXPORT SContainer
{
    std::string name;               ///< container name
    std::vector<std::string> lines; ///< lines read out from the container

    /// Print the contaner name and content
    void print() const
    {
        printf("Container Table [%s]\n", name.c_str());
        for (auto line : lines)
        {
            printf("%s\n", line.c_str());
        }
    }
};

class SIFI_EXPORT SParManager
{
  protected:
    std::string source;      ///< Parameters source file
    std::string destination; ///< Parameters destination file

    std::map<std::string, SContainer*> containers;               ///< Containers
    std::map<std::string, SParContainer*> par_containers;        ///< Par Containers
    std::map<std::string, SVirtualCalContainer*> cal_containers; ///< Par Containers
    std::map<std::string, SLookupTable*> lu_containers;          ///< Lookup Containers
    std::map<std::string, SPar*> parconts;                       ///< Parameters

    static SParManager* pm; ///< Instance of the SParManager

  private:
    // constructors
    SParManager() = default;
    SParManager(SParManager const&) = delete;

    SParManager& operator=(SParManager const&) = delete;

  public:
    // instance method
    static SParManager* instance();
    // destructor
    ~SParManager();

    // methods
    /// Set parameters source
    /// \param source source file name
    void setParamSource(const std::string& source) { this->source = source; }
    /// Set parameters destination
    /// \param dest destination file name
    void setParamDest(const std::string& dest) { this->destination = dest; }

    bool parseSource();
    void writeDestination();
    void writeContainers(const std::vector<std::string>& names);

    SContainer* getContainer(const std::string& cont_name);

    bool addParameterContainer(const std::string& cont_name, SPar* parcont);
    SPar* getParameterContainer(const std::string& cont_name);

    bool addLookupContainer(const std::string& cont_name, SLookupTable* lucont);
    SLookupTable* getLookupContainer(const std::string& cont_name);

    bool addCalibrationContainer(const std::string& cont_name, SVirtualCalContainer* calcont);
    SVirtualCalContainer* getCalibrationContainer(const std::string& cont_name);

    void print() const;
};

extern SIFI_EXPORT SParManager* pm();

#endif /* SPARMANAGER_H */
