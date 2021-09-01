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

#include <Rtypes.h> // for THashConsistencyHolder, ClassDef
#include <TObject.h>

#include <ctime>
#include <string> // for string, basic_string
#include <vector> // for vector

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * Simple structure to hold data validty range for a single version of container.
 */
struct validity_date_range
{
    std::time_t from{0};      ///< beginning of the range
    std::time_t to{0};        ///< end of the range
    std::time_t truncated{0}; ///< if truncate, stores the previous value

    /// Constructor
    /// \param l valid from time
    /// \param r valid to time
    validity_date_range()
    {
        from = std::time(nullptr);
        to = std::time(nullptr);
    }

    /// Constructor
    /// \param f valid from time
    /// \param t valid to time
    validity_date_range(std::time_t f, std::time_t t) : from(f), to(t) {}

    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param r range to compare
    /// \return whether the object is smaller than compared
    bool operator<(const validity_date_range& r) { return from < r.from; }
    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param time time to compare
    /// \return whether the object is smaller than compared
    bool operator<(std::time_t time) const { return from < time; }

    /// Checks whether #time is within the range
    /// \param time time to compare
    /// \return whether the #time is within the range
    bool operator==(std::time_t time) const { return from <= time and time < to; }

    /// Check for overlap
    /// \param range tested range
    /// \return overlap test result
    bool check_overlap(const validity_date_range& range) const { return from < range.to; }

    /// Check for overlap
    /// \param range tested range
    void truncate(const validity_date_range& range)
    {
        if (check_overlap(range))
        {
            truncated = to;
            to = range.from;
        }
    }
};

/**
 * Simple structure to hold data validty range for a single version of container.
 */
struct validity_runs_range
{
    uint64_t from{0};         ///< beginning of the range
    uint64_t to{0};           ///< end of the range
    std::time_t truncated{0}; ///< if truncate, stores the previous value

    /// Constructor
    /// \param l valid from time
    /// \param r valid to time
    validity_runs_range()
    {
        from = 0;
        to = 0;
    }

    /// Constructor
    /// \param f valid from time
    /// \param t valid to time
    validity_runs_range(uint64_t f, uint64_t t) : from(f), to(t) {}

    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param r range to compare
    /// \return whether the object is smaller than compared
    bool operator<(const validity_runs_range& r) { return from < r.from; }
    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param time time to compare
    /// \return whether the object is smaller than compared
    bool operator<(uint64_t time) const { return from < time; }

    /// Checks whether #time is within the range
    /// \param time time to compare
    /// \return whether the #time is within the range
    bool operator==(uint64_t time) const { return from <= time and time < to; }

    /// Check for overlap
    /// \param range tested range
    /// \return overlap test result
    bool check_overlap(const validity_runs_range& range) const { return from < range.to; }

    /// Check for overlap
    /// \param range tested range
    void truncate(const validity_runs_range& range)
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
inline bool operator<(const validity_date_range& lhs, const validity_date_range& rhs)
{
    return lhs.from < rhs.from;
}

/// Compare ranges, the smaller one has lower start time. Ranges may overlap.
/// \param lhs left range to compare
/// \param rhs right range to compare
/// return whether the object is smaller than compared
inline bool operator<(const validity_runs_range& lhs, const validity_runs_range& rhs)
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
    validity_runs_range validity;   ///< run validity start,end id

    void print(std::string_view name = {}) const;

    ClassDef(SContainer, 1);
};

#endif /* SCALCONTAINER_H */
