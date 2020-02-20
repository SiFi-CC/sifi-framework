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

#ifndef SPARMANAGER_H
// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#define SPARMANAGER_H

#include <map>

#include "SiFiManager.h"

class SParContainer;
class SPar;

class SParManager
{
protected:
    std::string source;             ///< Parameters source file
    std::string destination;        ///< Parameters destination file

    std::map<std::string, SParContainer *> containers;  ///< Containers
    std::map<std::string, SPar *> parconts;             ///< Parameters

    static SParManager * pm;        ///< Instance of the SParManager

private:
    // constructors
    SParManager();
    SParManager(SParManager const &) {}

    // methods
    /// Assignment operator
    /// \return this object
    SParManager & operator=(SParManager const &) { return *this; }

public:
    // instance method
    static SParManager * instance();
    // destructor
    ~SParManager();

    // methods
    /// Set parameters source
    /// \param source source file name
    void setParamSource(const std::string & source) { this->source = source; }
    /// Set parameters destination
    /// \param dest destination file name
    void setParamDest(const std::string & dest) { this->destination = dest; }

    bool parseSource();
    void writeDestination() const;

    bool addParameterContainer(const std::string & cont_name, SPar * parcont);
    SPar * getParameterContainer(const std::string & cont_name);

    void print() const;

private:
    /// Parser stepes
    enum WhatNext { WNContainer, WNContainerOrParam, WNParam, WNParamCont };
    WhatNext parseValues(const std::string & str, std::vector<std::string> & values);

};

extern SParManager * pm();

#endif /* SPARMANAGER_H */
