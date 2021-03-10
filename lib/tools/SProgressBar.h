// @(#)lib/tools:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SPROGRESSBAR_H
#define SPROGRESSBAR_H

#include "sifi_export.h"

#include <string>
#include <sys/types.h>

class SIFI_EXPORT SProgressBar
{
  protected:
    // members
    ulong cnt_current;  ///< current counter
    ulong cnt_previous; ///< previous counter
    ulong cnt_limit;    ///< counter limit
    uint point_width;   ///< width of a point
    uint bar_width;     ///< width of a bar
    bool new_bar;       ///< make new bar
    bool new_bar_line;  ///< make new bar line
    bool line_running;  ///< indicates that the bar line was not closed

    std::string line_prefix; ///< line prefix
    char bar_p;              ///< if counter below limit, use regular sign
    char alarm_p;            ///< if counter exceed limit, use alarm sign

  public:
    // constructor
    explicit SProgressBar(ulong limit, uint point_width = 500, uint bar_width = 20);

    // methods
    void reset();
    void finish();
    void setProgress(int current_location);

    ulong operator++();
    ulong operator++(int) = delete;

    /// Set line prefix
    /// \param prefix prefix
    void setLinePrefix(const std::string& prefix) { line_prefix = prefix; }
    /// Get line prefix
    /// \return current line prefix
    std::string getLinePrefix() const { return line_prefix; }

    /// Set bar character
    /// \param p character
    void setBarCharacter(char p) { bar_p = p; }
    /// Set alarm character
    /// \param p character
    void setAlarmCharacter(char p) { alarm_p = p; }

  private:
    void render();
};

#endif /* SPROGRESSBAR_H */
