// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParAsciiSource.h"
#include "SContainer.h"
#include "SHelperFunctions.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

/**
 * \class SParAsciiSource

\ingroup lib_base_database

Reads parameters from ascii file. Can be also used as a target to write parameters.
*/

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
SParAsciiSource::SParAsciiSource(const std::string& source) : SParSource(), source(source)
{
    parseSource();
}

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
SParAsciiSource::SParAsciiSource(std::string&& source) : SParSource(), source(source)
{
    parseSource();
}

/**
 * Parse source file
 *
 * \return success
 */
bool SParAsciiSource::parseSource()
{
    std::ifstream ifs(source);

    if (!ifs.is_open())
    {
        std::cerr << "Source file " << source << " could not be opened!" << std::endl;
        exit(EXIT_FAILURE);
    }

    size_t length = 0;
    if (ifs)
    {
        ifs.seekg(0, ifs.end);
        length = ifs.tellg();
        ifs.seekg(0, ifs.beg);
    }

    char* cbuff = new char[length];

    enum WhatNext
    {
        WNContainer,
        WNContainerOrParam,
        WNParam,
        WNParamCont
    } wn = WNContainer;

    std::string cont_name;
    SContainer* cont = nullptr;

    while (!ifs.eof())
    {
        ifs.getline(cbuff, length);

        std::string str(cbuff);
        SHelperFunctions::trim(str);
        SHelperFunctions::simplify(str);

        size_t pos = 0;

        // check if comment or empty line
        if (str[0] == '#' or (str.length() == 0 and wn != WNParamCont)) { continue; }
        // if container mark found, check whether it should be there, e.g. container after param new
        // line is forbidden
        else if (str[0] == '[')
        {
            if (wn == WNContainer or wn == WNContainerOrParam)
            {
                pos = str.find_first_of(']', 1);
                cont_name = str.substr(1, pos - 1);

                cont = new SContainer;
                cont->SetName(cont_name.c_str());
                cont->updated = true;
                containers.emplace(cont_name, std::unique_ptr<SContainer>(cont));

                wn = WNContainerOrParam;
                continue;
            }
            else
            {
                spdlog::error("[{0}] Didn't expected container here:\n {1}\n", __func__, str);
                delete[] cbuff;
                return false;
            }
        }
        else
        {
            // check if container name is found
            if (wn == WNContainer)
            {
                spdlog::error("[{0}] Expected container name here:\n {1}\n", __func__, str);
                delete[] cbuff;
                return false;
            }
            else if (wn == WNParam or wn == WNContainerOrParam)
            {
                cont->lines.push_back(str);
                wn = WNContainerOrParam;
            }
        }
    }

    delete[] cbuff;
    return true;
}

void SParAsciiSource::doPrint() const
{
    std::cout << "=== Ascii Source Info ===" << std::endl;
    std::cout << "    File name: " << source << std::endl;
    SParSource::print();
}

auto SParAsciiSource::doGetContainer(const std::string& name, ulong runid)
    -> std::shared_ptr<SContainer>
{
    auto it = containers.find(name);
    if (it == containers.end()) { return nullptr; }

    return it->second;
}

auto SParAsciiSource::doInsertContainer(const std::string& name, SContainer* cont) -> bool {}

auto SParAsciiSource::doInsertContainer(const std::string& name, std::vector<SContainer*> cont)
    -> bool
{
    // TODO implement this
    return false;
}

auto SParAsciiSource::doFindContainer(const std::string& name) -> bool
{
    auto it = containers.find(name);
    if (it != containers.end()) return true;

    return false;
}
