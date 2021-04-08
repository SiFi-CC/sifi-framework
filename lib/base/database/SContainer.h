// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCONTAINER_H
#define SCONTAINER_H

#include "sifi_export.h"

#include "SLookup.h"

#include <TObject.h>

#include <ctime>

struct validity_range_t
{
    std::time_t begin; ///< beginning of range
    std::time_t end;   ///< end of range

    /// Constructor
    /// \param l start time
    /// \param r end time
    validity_range_t()
    {
        begin = std::time(nullptr);
        end = std::time(nullptr);
    }

    /// Constructor
    /// \param l start time
    /// \param r end time
    validity_range_t(std::time_t l, std::time_t r) : begin(l), end(r) {}

    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param r range to compare
    /// return whether the object is smaller than compared
    bool operator<(const validity_range_t& r) { return begin < r.begin; }
    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param time time to compare
    /// return whether the object is smaller than compared
    bool operator<(std::time_t time) { return begin < time; }

    /// Checks whether #time is within the range
    /// \param time time to compare
    /// return whether the #time is within the range
    bool operator==(std::time_t time) { return begin <= time and time <= end; }
};

/// Compare ranges, the smaller one has lower start time. Ranges may overlap.
/// \param lhs left range to compare
/// \param rhs right range to compare
/// return whether the object is smaller than compared
inline bool operator<(const validity_range_t& lhs, const validity_range_t& rhs)
{
    return lhs.begin < rhs.begin;
}

/**
 * Template that contains basic calibation parameters. Consist of a N-parameters array:
 *  * #par,
 * which interpretation can be any, depends of the user.
 *
 * This structure must provie interface to read and write cal pars to a
 * container. Each deriving class must also provide proper interface.
 *  * read() - reads cal pars from container
 *  * write() - writes cal pars to the container
 *  * print() - print cal par values
 */
/**
 * Stores content of the container read out from the disk.
 */
class SIFI_EXPORT SContainer : public TObject
{
public:
    std::vector<std::string> lines; ///< lines read out from the container
    bool updated{false};            ///< flag -- was container updated?
    validity_range_t validity;      ///< run validity start,end id

    SContainer() = default;
    SContainer(const SContainer&) = default;

    /// Print the contaner name and content
    void print(std::string name = std::string()) const
    {
        printf("Container Table [%s]\n", name.c_str());
        for (auto line : lines)
        {
            printf("%s\n", line.c_str());
        }
    }

    ClassDefOverride(SContainer, 1);
};

#endif /* SCALCONTAINER_H */
