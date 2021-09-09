// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SRUNCONTAINER_H
#define SRUNCONTAINER_H

#include "sifi_export.h"

#include <TObject.h>

#include <ctime>
#include <tuple>

struct SIFI_EXPORT SRun : public TObject
{
    long runid;             ///< run id
    std::time_t start_time; ///< run timestamp in UTC
    std::time_t stop_time;  ///< run timestamp in UTC
    int runtype;            ///< run type

public:
    SRun() = default;

    /// Set run id
    /// \param id run id
    void setId(long id) { runid = id; }
    /// Get run id
    /// \return run id
    long getId() const { return runid; }

    /// Set run start time
    /// \param t start time
    void setStart(std::time_t t) { start_time = t; }
    /// Get run start time
    /// \return start time
    std::time_t getStart() const { return start_time; }
    /// Set run stop time
    /// \param t stop time
    void setStop(std::time_t t) { stop_time = t; }
    /// Get run stop time
    /// \return stop time
    std::time_t getStop() const { return stop_time; }

    /// Set run type
    /// \param t run type
    void setType(int t) { runtype = t; }
    /// Get run type
    /// \return run type
    long getType() const { return runtype; }

    /// Get all data as tuple
    /// \return tuple with data
    auto getData() -> std::tuple<long, std::time_t, std::time_t, long>
    {
        return std::make_tuple(runid, start_time, stop_time, runtype);
    }

    /// Print the contaner name and content
    void print() const;
};

#endif /* SRUNCALCONTAINER_H */
