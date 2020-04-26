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

#include <string>

class SProgressBar
{
protected:
    // members
    long cnt_current;           ///< current counter
    long cnt_previous;          ///< previous counter
    long cnt_limit;             ///< counter limit
    uint point_width;           ///< width of a point
    uint bar_width;             ///< width of a bar
    bool new_bar;               ///< make new bar
    bool new_bar_line;          ///< make new bar line

    std::string line_prefix;    ///< line prefix
    char bar_p;                 ///< ???
    char alarm_p;               ///< ???

public:
    // constructor
    explicit SProgressBar(ulong limit, uint point_width = 500, uint bar_width = 20);
    // destructor
    virtual ~SProgressBar() { close(); }

    // methods
    void close();
    void setProgress(int current_location);

    SProgressBar & operator++();
    SProgressBar operator++(int);

    /// Set line prefix
    /// \param prefix prefix
    void setLinePrefix(const std::string & prefix) { line_prefix = prefix; }
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
