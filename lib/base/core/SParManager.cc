// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParManager.h"
#include "SParContainer.h"
#include "SPar.h"
#include "SCalContainer.h"
#include "SLookup.h"

#include <algorithm>
#include <iterator>
#include <cctype>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>

/**
 * \class SParManager

\ingroup lib_base

Parameters Manager class. Stores and dumps all parameters used by the framework.

It's a singleton class of which object can be obtained using instance() method.

Paramaters mamager must be initializatied before
MDetectorManager::initParameterContainers() is called since it checks whether
the requested parameter containers exists.
*/

// for trim functions see
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

/**
 * Trim from start (in place)
 *
 * \param s string
 */
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

/**
 * Trim from end (in place)
 *
 * \param s string
 */
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

/**
 * Trim from both ends (in place)
 *
 * \param s string
 */
void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

/**
 * Trim from start (copying)
 *
 * \param s string
 * \return trimmed string
 */
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

/**
 * Trim from end (copying)
 *
 * \param s string
 * \return trimmed string
 */
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

/**
 * Trim from both ends (copying)
 *
 * \param s string
 * \return trimmed string
 */
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

/**
 * Remove all tabs (in place)
 *
 * \param s string
 */
void simplify(std::string & s)
{
    size_t pos = 0;
    while(1)
    {
        pos = s.find_first_of('\t', pos);
        if (pos == s.npos)
            return;
        s.replace(pos, 1, " ");
    }
}

/**
 * Check if float.
 *
 * \sa Stackoverflow #447206
 *
 * \param str string
 * \return is float
 */
bool isFloat(const std::string & str)
{
    std::istringstream iss(str);
    float f;
    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}

SParManager * SParManager::pm = nullptr;

/**
 * Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SParManager * SParManager::instance()
{
    if (!pm)
        pm = new SParManager;

    return pm;
}

/**
 * Shortcut
 * \return SParManager instance
 */
SParManager * pm()
{
    return SParManager::instance();
}

/**
 * Destructor
 */
SParManager::~SParManager()
{
    for (auto & c : parconts)
        delete c.second;

    for (auto & c : lu_containers)
        delete c.second;

    for (auto & c : cal_containers)
        delete c.second;

    for (auto & c : par_containers)
        delete c.second;

    for (auto & c : containers)
        delete c.second;
}

/**
 * Parse source file
 *
 * \return success
 */
bool SParManager::parseSource()
{
    std::ifstream ifs(source);
    
    if( ! ifs.is_open())
    {
        std::cerr << "Source file " << source <<
        " could not be opened!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    size_t length = 0;
    if (ifs)
    {
        ifs.seekg(0, ifs.end);
        length = ifs.tellg();
        ifs.seekg(0, ifs.beg);
    }

    char * cbuff = new char[length];

    enum WhatNext { WNContainer, WNContainerOrParam, WNParam, WNParamCont } wn = WNContainer;

    std::string cont_name;
    SContainer * cont = nullptr;

    while(!ifs.eof())
    {
        ifs.getline(cbuff, length);

        std::string str(cbuff);
        trim(str);
        simplify(str);

        size_t pos = 0;

        // check if comment or empty line
        if (str[0] == '#' or (str.length() == 0 and wn != WNParamCont))
        {
            continue;
        }
        // if container mark found, check whether it should be there, e.g. container after param new line is forbidden
        else if (str[0] == '[')
        {
            if (wn == WNContainer or wn == WNContainerOrParam)
            {
                pos = str.find_first_of(']', 1);
                cont_name = str.substr(1, pos-1);

                cont = new SContainer;
                cont->name = cont_name;
                containers.insert(std::pair<std::string, SContainer *>(cont_name, cont));

                wn = WNContainerOrParam;
                continue;
            }
            else
            {
                std::cerr << "Didn't expected container here: " << std::endl << str << std::endl;
                delete [] cbuff;
                return false;
            }
        }
        else
        {
            // check if container name is found
            if (wn == WNContainer)
            {
                std::cerr << "Expected container name here: " << std::endl << str << std::endl;
                delete [] cbuff;
                return false;
            }
            else if (wn == WNParam or wn == WNContainerOrParam)
            {
                cont->lines.push_back(str);
                wn = WNContainerOrParam;
            }
        }
    }

    delete [] cbuff;
    return true;
}

/**
 * Write all containers to destination file. Internally it creates a list of
 * containers and calls writeContainers();
 */
void SParManager::writeDestination() const
{
    std::vector<std::string> names;
    names.reserve(containers.size());

    std::transform(containers.begin(), containers.end(),
        std::back_inserter(names), [](std::pair<std::string, SContainer *> const & e) {
            return e.first; });

    writeContainers(names);
}

/**
 * Write selected containers to the destination file.
 *
 * \param names vector of container names
 */
void SParManager::writeContainers(const std::vector<std::string> & names) const
{
    for (const auto & pc : par_containers)
        pc.second->toContainer();

    for (const auto & cc : cal_containers)
        cc.second->toContainer();

    for (const auto & lc : lu_containers)
        lc.second->toContainer();

    if (!destination.size()) {
        std::cerr << "Destination file name is empty!" << std::endl;
        abort();
    }

    std::ofstream ofs(destination);
    if (ofs.is_open()) {
        for (auto &c : names) {
            ofs << "[" << c << "]" << std::endl;
            for (const auto & l : containers.at(c)->lines)
                ofs << l << std::endl;
        }
    }
    ofs.close();
}


/**
 * Print containers
 */
void SParManager::print() const
{
    for (const auto & p : par_containers)
        p.second->print();
}

/**
 * Get parameter container by name.
 *
 * \param cont_name container name
 * \return pointer to container
 */
SContainer * SParManager::getContainer(const std::string& cont_name)
{
    SContainer * cont = containers[cont_name];
    if (!cont)
    {
        std::cerr << "Container " << cont_name << " doesn't exists!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return cont;
}

/**
 * Add new parameter container.
 *
 * \param cont_name container name
 * \param parcont container object
 * \return success
 */
bool SParManager::addParameterContainer(const std::string& cont_name, SPar* parcont)
{
    SContainer * cont = containers[cont_name];
    if (!cont)
    {
        std::cerr << "Container " << cont_name << " doesn't exists!" << std::endl;
        exit(EXIT_FAILURE);
    }

    SParContainer * pc = par_containers[cont_name];
    if (!pc) {
        pc = new SParContainer(cont_name);
        par_containers[cont_name] = pc;

        pc->fromContainer();
    }

    parcont->clear();
    bool ret = parcont->getParams(pc);
    if (!ret)
    {
        std::cerr << "Initialization of " << cont_name << " param container failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    parconts.insert(std::pair<std::string, SPar*>(cont_name, parcont));
    return true;
}

/**
 * Get parameter container by name.
 *
 * \param cont_name container name
 * \return pointer to container
 */
SPar * SParManager::getParameterContainer(const std::string& cont_name)
{
    return parconts[cont_name];
}


/**
 * Add new lookup table container.
 *
 * \param cont_name container name
 * \param lucont lookup table object
 * \return success
 */
bool SParManager::addLookupContainer(const std::string& cont_name, SLookupTable* lucont)
{
    SContainer * cont = containers[cont_name];
    if (!cont)
    {
        std::cerr << "Container " << cont_name << " doesn't exists!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (lucont) {
        lu_containers[cont_name] = lucont;
    }

    return true;
}

/**
 * Get lookup table by name.
 *
 * \param cont_name container name
 * \return pointer to container
 */
SLookupTable * SParManager::getLookupContainer(const std::string& cont_name)
{
    return lu_containers[cont_name];
}

/**
 * Add new calibration container.
 *
 * \param cont_name container name
 * \param calcont calibration object
 * \return success
 */
bool SParManager::addCalibrationContainer(const std::string& cont_name, SVirtualCalContainer* calcont)
{
    SContainer * cont = containers[cont_name];
    if (!cont)
    {
        std::cerr << "Container " << cont_name << " doesn't exists!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (calcont) {
        cal_containers[cont_name] = calcont;
    }

    return true;
}

/**
 * Get calibration container by name.
 *
 * \param cont_name container name
 * \return pointer to container
 */
SVirtualCalContainer * SParManager::getCalibrationContainer(const std::string& cont_name)
{
    return cal_containers[cont_name];
}
