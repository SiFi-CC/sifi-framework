// @(#)lib/tools:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SProgressBar.h"

#include <iostream>

/** \class SProgressBar
\ingroup lib_tools

Display progress of the analysis.

*/

/** Constructor
 * \param cnt_limit limit of the bar
 * \param point_width width of the point
 * \param bar_width width of teh bar
 */
SProgressBar::SProgressBar(ulong limit, uint point_width, uint bar_width)
    : cnt_current(0), cnt_previous(0), cnt_limit(limit)
    , point_width(point_width), bar_width(bar_width)
    , new_bar(true), new_bar_line(true)
    , line_prefix("==> Processing data ")
    , bar_p('.'), alarm_p('!')
{
}

/** Close bar line
 */
void SProgressBar::close() const
{
    if (cnt_current >= cnt_limit)
        return;

    double num_percent = 100.0*(cnt_current)/cnt_limit;
    std::cout << " " << cnt_current << " (" << num_percent << "%)" << "\n" << std::flush;
}

/** Set current progress status
 * \param current_location current position
 */
void SProgressBar::setProgress(int current_location)
{
    cnt_current = current_location;
    render();
}

/** Increase progress
 */
SProgressBar & SProgressBar::operator++()
{
    ++cnt_current;
    render();

    return *this;
}

/** Increase progress
 */
SProgressBar SProgressBar::operator++(int)
{
    SProgressBar pb(*this);
    ++(*this);

    return pb;
}

/** Render bar
 */
void SProgressBar::render()
{
    for (long int i = cnt_previous; i < cnt_current; ++i)
    {
        if (new_bar or new_bar_line)
        {
            std::cout << line_prefix << std::flush;
            new_bar = false;
            new_bar_line = false;
        }

        if (i != 0  and ( (i+1) % point_width ) == 0)
        {
            if (i < cnt_limit)
                std::cout << bar_p << std::flush;
            else
                std::cout << alarm_p << std::flush;
        }

        if ((i != 0  and (i+1) % (point_width * bar_width) == 0) or
            (i == (cnt_limit-1)))
        {
            double num_percent = 100.0*(i+1)/cnt_limit;
            std::cout << " " << i+1 << " (" << num_percent << "%) " << "\n" << std::flush;

            new_bar_line = true;
        }
    }

    cnt_previous = cnt_current;
}
