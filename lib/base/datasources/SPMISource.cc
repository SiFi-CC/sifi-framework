// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SPMISource.h"
#include "SUnpacker.h"

#include <algorithm>          // for max, min
#include <cmath>              // for fabs
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for abort
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <map>
#include <utility> // for pair
#include <vector>  // for vector

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SPMISource::SPMISource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), istream(), state(INIT), hit_cache(nullptr)
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SPMISource::open()
{
    istream.open(input.c_str());
    if (!istream.is_open())
    {
        std::cerr << "##### Error in SPMISource::open()! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }

    std::string header;
    getline(istream, header);

    if (unpackers.size() == 0) return false;

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();

        bool res = unpackers[subevent]->init();
        if (!res)
        {
            printf("Forced unpacker %#x not initalized\n", subevent);
            abort();
        }
    }
    else
    {
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
        {
            bool res = iter->second->init();
            if (!res)
            {
                printf("Unpacker %#x not initalized\n", iter->first);
                abort();
            }
        }
    }
    return true;
}

bool SPMISource::close()
{
    if (subevent != 0x0000)
    {
        if (unpackers[subevent])
            unpackers[subevent]->finalize();
        else
            abort();
    }
    else
    {
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->finalize();
    }

    istream.close();
    return true;
}

bool SPMISource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;

    const double deltaT = 10; // 10 ns window
    const double ps_to_ns = 1E3;
    double time_first = 0;

    std::vector<std::shared_ptr<PMIHit>> hits;

    if (state == DONE) { return false; }

    if (state == INIT)
    {
        hit_cache = std::make_shared<PMIHit>();
        istream >> hit_cache->fiberID >> hit_cache->time_l >> hit_cache->time_r >>
            hit_cache->qdc_l >> hit_cache->qdc_r;

        hit_cache->time_l /= ps_to_ns;
        hit_cache->time_r /= ps_to_ns;

        state = READING;
    }

    if (state == READING)
    {
        double cache_time = (hit_cache->time_l + hit_cache->time_r) / 2.;
        time_first = std::min(hit_cache->time_l, hit_cache->time_r);
        // hit_cache->time_l -= time_first;
        // hit_cache->time_r -= time_first;

        hits.push_back(hit_cache);

        while (true)
        {
            auto hit_current = std::make_shared<PMIHit>();

            istream >> hit_current->fiberID >> hit_current->time_l >> hit_current->time_r >>
                hit_current->qdc_l >> hit_current->qdc_r;

            hit_current->time_l /= ps_to_ns;
            hit_current->time_r /= ps_to_ns;

            if (!istream.good())
            {
                state = DONE;
                break;
            }

            double current_time = (hit_current->time_l + hit_current->time_r) / 2.;

            if (fabs(cache_time - current_time) < deltaT) // same event?
            {
                // hit_current->time_l -= time_first;
                // hit_current->time_r -= time_first;
                hits.push_back(hit_current);
            }
            else // next event
            {
                hit_cache = hit_current;
                break;
            }
        }
    }

    int n_hits = hits.size();

    for (int i = 0; i < n_hits; i++)
    {
        hits[i]->time_l -= time_first;
        hits[i]->time_r -= time_first;
    }

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setSampleTimeBin(1.);
        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < n_hits; i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, 0, subevent, hits[i].get(), 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
            u.second->setSampleTimeBin(1.);
            u.second->setADCTomV(1.);
            for (int i = 0; i < n_hits; i++)
            {
                u.second->execute(0, 0, u.first, hits[i].get(), 0);
            }
        }
    }

    return true;
}

/**
 * Set input for the source.
 *
 * \param filename input file name
 * \param length length of buffer to read
 */
void SPMISource::setInput(const std::string& filename, size_t length) { input = filename; }
