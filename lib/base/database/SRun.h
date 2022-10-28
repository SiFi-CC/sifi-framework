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

/**
 * \class SRun
 * Store information about the single acquisition store.
 *
 * The run can be marked invalid, and as such, can/should be ignored by analysis.
 */
struct SIFI_EXPORT SRun : public TObject
{
    ulong id{0};              ///< run id
    std::time_t start_time{}; ///< run timestamp in UTC
    std::time_t stop_time{};  ///< run timestamp in UTC
    int type{0};              ///< run type
    enum class Status
    {
        Valid,
        Invalid
    } status{Status::Invalid}; ///< run is invalid
    std::string file_name;

public:
    SRun() = default;

    /// Mark run as Valid
    void markValid() { status = Status::Valid; }
    /// Mark run as Invalid
    void markInvalid() { status = Status::Invalid; }
    /// Get run status
    auto getStatus() const -> Status { return status; };

    /// Print the contaner name and content
    void print() const;

    ClassDef(SRun, 0);
};

struct SIFI_EXPORT SExperiment : public TObject
{
    SExperiment() = default;
    SExperiment(std::string name, std::string start, std::string close)
        : TObject(), name(name), start_date(start), close_date(close)
    {
    }

    std::string name;
    std::string start_date;
    std::string close_date;

    ClassDef(SExperiment, 0);
};

#endif /* SRUNCALCONTAINER_H */
