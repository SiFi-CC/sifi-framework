// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>

#include "SLookupManager.h"
#include "SLookupContainer.h"
#include "SLookup.h"

/** \class SLookupManager
\ingroup lib_base

Parameters Manager class. Stores and dumps all parameters used by the framework.

It's a singleton class of which object can be obtained using instance() method.

Paramaters mamager must be initializatied before
MDetectorManager::initParameterContainers() is called since it checks whether
the requested parameter containers exists.
*/

extern void trim(std::string &s);
extern void simplify(std::string & s);
extern bool isFloat(const std::string & str);

SLookupManager * SLookupManager::lm = nullptr;

/** Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SLookupManager * SLookupManager::instance()
{
    if (!lm)
        lm = new SLookupManager;

    return lm;
}

/** Shortcut
 * \return SLookupManager instance
 */
SLookupManager * lm()
{
    return SLookupManager::instance();
}

/** Default constructor
 */
SLookupManager::SLookupManager()
{
}

/** Destructor
 */
SLookupManager::~SLookupManager()
{
}

/** Parse source file
 *
 * \return success
 */
bool SLookupManager::parseSource()
{
    std::ifstream ifs(source);
    if (!ifs.is_open())
    {
        std::cerr << "Source " << source << " could not be open!" << std::endl;
        abort();
    }
    size_t length = 0;
    if (ifs)
    {
        ifs.seekg(0, ifs.end);
        length = ifs.tellg();
        ifs.seekg(0, ifs.beg);
    }

    char * cbuff = new char[length];

    WhatNext wn = WNContainer;

    std::string cont_name;
    std::string param_name;
    std::string type_name;
    std::vector<std::string> values;

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
//                 printf("Found container %s\n", cont_name.c_str());

                addLookupContainer(cont_name, new SLookupContainer(cont_name));

                param_name.clear();

                wn = WNContainerOrParam;
                continue;
            }
            else
            {
                std::cerr << "Didn't expected container here: " << std::endl << str << std::endl;
                return false;
            }
        }
        else
        {
            // check if container name
            if (wn == WNContainer)
            {
                std::cerr << "Expected container name here: " << std::endl << str << std::endl;
                return false;
            }
            else if (wn == WNParam or wn == WNContainerOrParam)
            {
                containers[cont_name]->addLine(str);
                wn = WNContainerOrParam;
            }
        }
    }

    return true;
}

/** Print containers
 */
void SLookupManager::print() const
{
     std::map<std::string, SLookupContainer *>::const_iterator it = containers.begin();
     for (; it != containers.end(); ++it)
         it->second->print();
}

/** Add new parameter container.
 *
 * \param cont_name container name
 * \param parcont container object
 * \return success
 */
bool SLookupManager::addLookupContainer(const std::string& cont_name, SLookupContainer* lookupcont)
{
//     FIXME
//     SLookupContainer * lc = containers[cont_name];
//     if (!lc)
//     {
//         std::cerr << "Container " << cont_name << " doesn't exists!" << std::endl;
//         exit(EXIT_FAILURE);
//     }
//
//     lookupcont->clear();
//     bool ret = lookupcont->getParams(lc);
//     if (!ret)
//     {
//         std::cerr << "Initialization of " << cont_name << " param container failed!" << std::endl;
//         exit(EXIT_FAILURE);
//     }
//
    containers.insert(std::pair<std::string, SLookupContainer*>(cont_name, lookupcont));
    return true;
}

/** Get parameter container by name.
 *
 * \param cont_name container name
 * \return pointer to container
 */
SLookupContainer * SLookupManager::getLookupContainer(const std::string& cont_name)
{
    return containers[cont_name];
}
