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

#include <cstdio>
#include <ctime>
#include <tuple>

struct SIFI_EXPORT SRunContainer : public TObject
{
    long runid;                ///< run id
    std::time_t run_starttime; ///< run timestamp in UTC
    std::time_t run_stoptime;  ///< run timestamp in UTC
    int runtype;               ///< run type

public:
    SRunContainer();
    SRunContainer(const SRunContainer&) = default;

    /// Set run id
    /// \param id run id
    void setId(long id) { runid = id; }
    /// Get run id
    /// \return run id
    long getId() const { return runid; }

    /// Set run start time
    /// \param t start time
    void setStart(std::time_t t) { run_starttime = t; }
    /// Get run start time
    /// \return start time
    std::time_t getStart() const { return run_starttime; }
    /// Set run stop time
    /// \param t stop time
    void setStop(std::time_t t) { run_stoptime = t; }
    /// Get run stop time
    /// \return stop time
    std::time_t getStop() const { return run_stoptime; }

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
        return std::make_tuple(runid, run_starttime, run_stoptime, runtype);
    }

    /// Print the contaner name and content
    void print() const
    {
        std::printf("=== Run info === id: %ld  start: %s  stop: %s  type: %d\n", runid,
                    asctime(gmtime(&run_starttime)), asctime(gmtime(&run_stoptime)), runtype);
    }
};

#endif /* SRUNCALCONTAINER_H */
