// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SHELPERFUNCTIONS_H
#define SHELPERFUNCTIONS_H

#include <algorithm>
#include <cstddef>
#include <ctype.h>
#include <sstream>
#include <string>

namespace SHelperFunctions
{
/**
 * Trim from start (in place)
 *
 * \param s string
 */
inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

/**
 * Trim from end (in place)
 *
 * \param s string
 */
inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

/**
 * Trim from both ends (in place)
 *
 * \param s string
 */
inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

/**
 * Trim from start (copying)
 *
 * \param s string
 * \return trimmed string
 */
inline std::string ltrim_copy(std::string s)
{
    ltrim(s);
    return s;
}

/**
 * Trim from end (copying)
 *
 * \param s string
 * \return trimmed string
 */
inline std::string rtrim_copy(std::string s)
{
    rtrim(s);
    return s;
}

/**
 * Trim from both ends (copying)
 *
 * \param s string
 * \return trimmed string
 */
inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
}

/**
 * Remove all tabs (in place)
 *
 * \param s string
 */
inline void simplify(std::string& s)
{
    size_t pos = 0;
    while (1)
    {
        pos = s.find_first_of('\t', pos);
        if (pos == s.npos) return;
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
inline bool isFloat(const std::string& str)
{
    std::istringstream iss(str);
    float f;
    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}

}; // namespace SHelperFunctions

#endif /* SHELPERFUNCTIONS_H */
