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

/**
 * \class SProgressBar
\ingroup lib_tools

Display progress of the analysis.

*/

/**
 * Constructor
 *
 * \param limit expected limit of the bar
 * \param point_width width of the point
 * \param bar_width number of points per line
 */
SProgressBar::SProgressBar(ulong limit, uint point_width, uint bar_width)
    : cnt_current(0), cnt_previous(0), cnt_limit(limit), point_width(point_width),
      bar_width(bar_width), new_bar(true), new_bar_line(true), line_running(false),
      line_prefix("==> Processing data "), bar_p('.'), alarm_p('!')
{
}

/**
 * Close bar line if line is running
 */
void SProgressBar::finish()
{
    if (!line_running) return;

    double num_percent = 100.0 * (cnt_current) / cnt_limit;
    std::cout << " " << cnt_current << " (" << num_percent << "%)" << std::endl << std::flush;

    line_running = false;
}

/**
 * Set current progress status
 *
 * \param current_location current position
 */
void SProgressBar::setProgress(int current_location)
{
    cnt_current = current_location;
    render();
}

/**
 * Increase progress bar.
 *
 * \return instance of the Progress Bar object
 */
ulong SProgressBar::operator++()
{
    ++cnt_current;
    render();

    return cnt_current;
}

/**
 * Render bar
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

        if (i != 0 and (i + 1) % point_width == 0)
        {
            if (i < cnt_limit)
                std::cout << bar_p << std::flush;
            else
                std::cout << alarm_p << std::flush;
            line_running = true;
        }
        else
        {
            line_running = false;
        }

        if ((i != 0 and (i + 1) % (point_width * bar_width) == 0))
        {
            double num_percent = 100.0 * (i + 1) / cnt_limit;
            std::cout << " " << i + 1 << " (" << num_percent << "%) " << std::endl << std::flush;

            new_bar_line = true;
        }
    }
    cnt_previous = cnt_current;
}

/**
 * Reset the bar. First it will close running bar, and then reset counters to 0
 */
void SProgressBar::reset()
{
    finish();

    cnt_current = 0;
    cnt_previous = 0;

    new_bar = true;
    new_bar_line = true;
}
