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
    std::time_t from{0};      ///< beginning of the range
    std::time_t to{0};        ///< end of the range
    std::time_t truncated{0}; ///< if truncate, stores the previous value

    /// Constructor
    /// \param l valid from time
    /// \param r valid to time
    validity_range_t()
    {
        from = std::time(nullptr);
        to = std::time(nullptr);
    }

    /// Constructor
    /// \param l valid from time
    /// \param r valid to time
    validity_range_t(std::time_t f, std::time_t t) : from(f), to(t) {}

    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param r range to compare
    /// return whether the object is smaller than compared
    bool operator<(const validity_range_t& r) { return from < r.from; }
    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param time time to compare
    /// return whether the object is smaller than compared
    bool operator<(std::time_t time) const { return from < time; }

    /// Checks whether #time is within the range
    /// \param time time to compare
    /// return whether the #time is within the range
    bool operator==(std::time_t time) const { return from <= time and time < to; }

    /// Check for overlap
    /// \param range tested range
    /// \return overlap test result
    bool check_overlap(const validity_range_t& range) const { return from < range.to; }

    /// Check for overlap
    /// \param range tested range
    /// \return overlap test result
    void truncate(const validity_range_t& range)
    {
        if (check_overlap(range))
        {
            truncated = to;
            to = range.from;
        }
    }
};

/// Compare ranges, the smaller one has lower start time. Ranges may overlap.
/// \param lhs left range to compare
/// \param rhs right range to compare
/// return whether the object is smaller than compared
inline bool operator<(const validity_range_t& lhs, const validity_range_t& rhs)
{
    return lhs.from < rhs.from;
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

    void print(std::string name = std::string()) const;

    ClassDef(SContainer, 1);
};

#endif /* SCALCONTAINER_H */
