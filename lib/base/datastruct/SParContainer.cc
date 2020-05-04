// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParContainer.h"

#include "SParManager.h"

#include <iostream>
#include <sstream>
#include <string>

/**
 * \class SParContainer
\ingroup lib_base

SPar is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SParManager and stored in the
SParContainer. The getParam() method reads content of the SParContainer and
fills variables inside the SPar object. The putParam method allows to update
parameters in the container and write to param file.

\sa SFibersStackCalibratorPar
\sa SFibersStackDigitizerPar
\sa SFibersStackGeomPar
*/

namespace {

enum WhatNext { WNContainer, WNContainerOrParam, WNParam, WNParamCont };

/**
 * Parse single value.
 *
 * \param str string with values
 * \param values vector to store values
 * \return next parsing step
 */
WhatNext parseValues(const std::string & str, std::vector<std::string> & values)
{
    size_t pos2 = 0;
    while(true)
    {
        size_t pos = str.find_first_not_of(' ', pos2);

        // if end of line
        if (pos == str.npos)
        {
            break;
        }

        // if new line detected
        if (str[pos] == '\\')
        {
            return WNParamCont;
        }

        pos2 = str.find_first_of(' ', pos+1);
        std::string match = str.substr(pos, pos2-pos);
        if (isFloat(match))
        {
            values.push_back(match);
        }
        else
        {
            std::cerr << "Value is not a number:" << std::endl << str << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return WNParam;
}

};

/**
 * Constructor
 *
 * \param container container name
 */
SParContainer::SParContainer(const std::string& container) : container(container), line_split(8)
{
}

/**
 * Add key with integer value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::add(const std::string & name, Int_t val)
{
    std::stringstream buff;
    buff << "  " << val;
    std::vector<std::string> v;
    v.push_back(buff.str());
    parameters[name] = TypeDataField("Int_t", v);

    return true;
}

/**
 * Add key with float value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::add(const std::string & name, Float_t val)
{
    std::stringstream buff;
    buff << "  " << val;
    std::vector<std::string> v;
    v.push_back(buff.str());
    parameters[name] = TypeDataField("Float_t", v);

    return true;
}

/**
 * Add key with double precision float value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::add(const std::string & name, Double_t val)
{
    std::stringstream buff;
    buff << "  " << val;
    std::vector<std::string> v;
    v.push_back(buff.str());
    parameters[name] = TypeDataField("Double_t", v);

    return true;
}

/**
 * Add key with integer array value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::add(const std::string & name, const TArrayI & val)
{
    std::stringstream buff;
    std::vector<std::string> v;
    for (int i = 0; i < val.GetSize(); ++i)
    {
        buff << "  " << val[i];
        v.push_back(buff.str());
    }
    parameters[name] = TypeDataField("Int_t", v);

    return true;
}

/**
 * Add key with float array value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::add(const std::string & name, const TArrayF & val)
{
   std::stringstream buff;
   std::vector<std::string> v;
    for (int i = 0; i < val.GetSize(); ++i)
    {
        buff << "  " << val[i];
        v.push_back(buff.str());
    }
    parameters[name] = TypeDataField("Float_t", v);

    return true;
}

/**
 * Add key with double precision float array value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::add(const std::string & name, const TArrayD & val)
{
    std::stringstream buff;
    std::vector<std::string> v;
    for (int i = 0; i < val.GetSize(); ++i)
    {
        buff << "  " << val[i];
        v.push_back(buff.str());
    }
    parameters[name] = TypeDataField("Double_t", v);

    return true;
}

/**
 * Get key with integer value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::fill(const std::string & name, Int_t& val)
{
    ParMap::const_iterator it = parameters.find(name);

    if (it == parameters.end())
    {
        std::cerr << "Parameter name " << name << " doesn't exists in " << container << "!" << std::endl;
        return false;
    }
    if (it->second.first != "Int_t")
    {
        std::cerr << "Incorrect type for parameter name " << name << std::endl;
        return false;
    }

    std::stringstream buff(it->second.second[0]);
    buff >> val;

    return true;
}

/**
 * Get key with float value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::fill(const std::string & name, Float_t& val)
{
    ParMap::const_iterator it = parameters.find(name);

    if (it == parameters.end())
    {
        std::cerr << "Parameter name " << name << " doesn't exists in " << container << "!" << std::endl;
        return false;
    }
    if (it->second.first != "Float_t")
    {
        std::cerr << "Incorrect type for parameter name " << name << std::endl;
        return false;
    }

    std::stringstream buff(it->second.second[0]);
    buff >> val;

    return true;
}

/**
 * Get key with double precision float value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::fill(const std::string & name, Double_t& val)
{
    ParMap::const_iterator it = parameters.find(name);

    if (it == parameters.end())
    {
        std::cerr << "Parameter name " << name << " doesn't exists in " << container << "!" << std::endl;
        return false;
    }
    if (it->second.first != "Double_t")
    {
        std::cerr << "Incorrect type for parameter name " << name << std::endl;
        return false;
    }

    std::stringstream buff(it->second.second[0]);
    buff >> val;

    return true;
}

/**
 * Get key with integer array value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::fill(const std::string & name, TArrayI& val)
{
    ParMap::const_iterator it = parameters.find(name);

    if (it == parameters.end())
    {
        std::cerr << "Parameter name " << name << " doesn't exists in " << container << "!" << std::endl;
        return false;
    }
    if (it->second.first != "Int_t")
    {
        std::cerr << "Incorrect type for parameter name " << name << std::endl;
        return false;
    }

    for (int i = 0; i < it->second.second.size(); ++i)
        val[i] = std::stoi(it->second.second[i].c_str());

    return true;
}

/**
 * Get key with float array value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::fill(const std::string & name, TArrayF& val)
{
    ParMap::const_iterator it = parameters.find(name);

    if (it == parameters.end())
    {
        std::cerr << "Parameter name " << name << " doesn't exists in " << container << "!" << std::endl;
        return false;
    }
    if (it->second.first != "Float_t")
    {
        std::cerr << "Incorrect type for parameter name " << name << std::endl;
        return false;
    }

    for (int i = 0; i < it->second.second.size(); ++i)
        val[i] = std::stof(it->second.second[i].c_str());

    return true;
}

/**
 * Get key with double precision float array value
 *
 * \param name key name
 * \param val value
 * \return success
 */
bool SParContainer::fill(const std::string & name, TArrayD& val)
{
    ParMap::const_iterator it = parameters.find(name);

    if (it == parameters.end())
    {
        std::cerr << "Parameter name " << name << " doesn't exists in " << container << "!" << std::endl;
        return false;
    }
    if (it->second.first != "Double_t")
    {
        std::cerr << "Incorrect type for parameter name " << name << std::endl;
        return false;
    }

    for (int i = 0; i < it->second.second.size(); ++i)
        val[i] = std::stod(it->second.second[i].c_str());

    return true;
}

/**
 * Print container
 */
void SParContainer::print()
{
    printf("Container [%s]\n", container.c_str());
    for (const auto & p : parameters)
    {
        printf("%s:  %s", p.first.c_str(), p.second.first.c_str());
        for (int i = 0; i < p.second.second.size(); ++i)
        {
            printf("  %s", p.second.second[i].c_str());
        }
        putchar('\n');
    }
}

/**
 * Init param with type and values
 *
 * \param name key name
 * \param type key type
 * \param values key values
 * \return success
 */
bool SParContainer::initParam(const std::string& name, const std::string& type, const std::vector<std::string> & values)
{
    parameters[name] = TypeDataField(type, values);
    return true;
}

/**
 * Parses all the parameters from the file. In case of parsing fail (broken
 * input file), the function aborts the execution of the calling process.
 */
void SParContainer::fromContainer() {
    SContainer * sc = pm()->getContainer(container);
    if (!sc) throw "No parameter container.";

    WhatNext wn = WNParam;
    std::string param_name;
    std::string type_name;
    std::vector<std::string> values;

    for (auto line : sc->lines) {
        trim(line);
        simplify(line);

        // skip comment or empty line
        if (line[0] == '#' or (line.length() == 0 and wn != WNParamCont))
        {
            continue;
        }
        else
        {
            size_t pos2 = 0;
            if (wn == WNParam)
            {
                // find parameter name ended with :
                size_t pos = line.find_first_of(':', 1);
                param_name = line.substr(0, pos);
                trim(param_name);

                // prepare other variables
                type_name.clear();
                values.clear();

                // find type name
                pos = line.find_first_not_of(' ', pos+1);

                // type name must be in the same line like param name
                if (line[pos] == '\\')
                {
                    std::cerr << "No type name detected in the param name line:" << std::endl << line << std::endl;
                    abort();
                }
                pos2 = line.find_first_of(' ', pos+1);

                type_name = line.substr(pos, pos2-pos);
                if (type_name != "Int_t" and type_name != "Float_t" and type_name != "Double_t")
                {
                    std::cerr << "Invalid param type '" << type_name << "' in line:" << std::endl << line << std::endl;
                    abort();
                }

                wn = WNParamCont;
            }

            if (wn == WNParamCont)
            {
                wn = ::parseValues(line.substr(pos2, -1), values);
                if (wn == WNParam)
                {
                    if (values.size() == 0)
                    {
                        std::cerr << "No values given in line:" << std::endl << line << std::endl;
                        abort();
                    }
                    else
                    {
                        initParam(param_name, type_name, values);
                    }
                }
            }
        }
    }
}

/**
 * Exports Par Container to the corresponding SContainer class. It formats each
 * parameter with format string `"%s : %s", name, type`, followed by list of
 * parameter values. If the number of parameters is not larger than #line_split
 * value, the values follow in the same line, otherwise are printed in the next
 * lines, each containing maximal of #line_split values. In that case each line
 * beside the last one is broke with `\` character.
 */
void SParContainer::toContainer() const {
    SContainer * sc = pm()->getContainer(container);
    if (!sc) throw "No parameter container.";

    sc->lines.clear();

    const size_t len = 1024;
    char buff[len];
    for (const auto & p : parameters) {
        snprintf(buff, len, "%s: %s ", p.first.c_str(), p.second.first.c_str());
        std::string s(buff);
        const TypeDataField & df = p.second;
        size_t len_d = df.second.size();

        if (len_d <= line_split) {
            for (const auto & pd : df.second) {
                s += "  ";
                s += pd;
            }
            sc->lines.push_back(s);
        } else {
            size_t cnt = 0;
            for (const auto & pd : df.second) {
                if (cnt % line_split == 0) {
                    s += " \\";
                    sc->lines.push_back(s);
                    s.clear();
                }
                s += "  ";
                s += pd;
                ++cnt;
            }
            sc->lines.push_back(s);
        }
    }
}
