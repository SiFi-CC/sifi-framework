// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SHLDSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <sstream>
#include <map>

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SHLDSource::SHLDSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), istream()
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SHLDSource::open()
{
    ref = hadaq::ReadoutHandle::Connect(input.c_str() );

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

bool SHLDSource::close()
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
    return true;
}

bool SHLDSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;
    std::vector<HLDHit> hits;
//    if(getCurrentEvent() > tree->GetEntries() ) {
//        std::cerr << "##### Error in " << __PRETTY_FUNCTION__ << "! Invalid HLD entry" << std::endl;
//        return false;
//    }
    hadaq::RawEvent *evnt;
    if((evnt = ref.NextEvent(1.) ) == 0) return false;
    hadaq::RawSubevent *sub = 0;
    while ((sub = evnt->NextSubevent(sub) ) != 0) {
        unsigned trbSubEvSize = sub->GetSize() / 4 - 4;
        unsigned ix = 0;
        while (ix < trbSubEvSize) {
            unsigned data = sub->Data(ix++);
            unsigned datakind = data & 0xFFFF;
            if(datakind != 0xbeef) continue;
            unsigned len = (data >> 16) & 0xFFFF;
            unsigned epoch = 0;
            double last_rising[32] = {0}, last_falling[32] = {0}, tot[32] = {0};
            HLDHit hit_cache;
            for(unsigned cnt=0; cnt < len; cnt++, ix++) {
                unsigned msg = sub->Data(ix);    
                if((msg & 0xe0000000) == 0x60000000) {
                    //epoch is not used at the moment
                    epoch = msg & 0xFFFFFFF;
                } else if((msg & 0xe0000000) == 0x80000000) {
                    unsigned datalen = (msg >> 16) & 0xFFFF;
                    Int_t channel = (msg >> 22) & 0x7F;
                    unsigned isrising = (msg >> 11) & 0x1;
                    unsigned coarse = (msg & 0x7FF);
                    unsigned fine = (msg >> 12) & 0x3FF;
                    Double_t tm = coarse * 5.; //bins to ns
                    tm -= (fine > 31 ? fine - 31 : 0) / (0. + 491 - 31) * 5.; //simple approximation of fine time from range 31 - 491, uncalibrated fine time
                    if (isrising) {
                        last_rising[channel] = tm;
                    } else {
                        last_falling[channel] = tm;
                        if (last_rising[channel] > 0) {
                            tot[channel] = last_falling[channel] - last_rising[channel];
                            last_rising[channel] = 0;                            
                        }
                    }
                    //printf("cnt=%d msg=%x datakind=%x ch=%d is_rising=%d epoch=%d coarse=%x fine=%x stamp=%f tot=%f\n", cnt, msg, datakind, channel, isrising, epoch, coarse, fine, tm, tot[channel]);
                    if(channel==2) {
                        hit_cache.time_l = last_rising[channel];
                        hit_cache.qdc_l = tot[channel];
                    }
                    if(channel==18) {
                        hit_cache.time_r = last_rising[channel];
                        hit_cache.qdc_r = tot[channel];
                    }
                    hit_cache.fiberID = 0;
                }
            }
            if(hit_cache.time_l > 1  && hit_cache.time_r > 1 && hit_cache.qdc_l > 1  && hit_cache.qdc_r > 1)
                hits.push_back(hit_cache);
            ix+=len;
        }
    }

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
//        unpackers[subevent]->setSampleTimeBin(1.);
//        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < hits.size(); i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, 0, subevent, &hits[i], 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
//            u.second->setSampleTimeBin(1.);
//            u.second->setADCTomV(1.);
            for (int i = 0; i < hits.size(); i++)
            {
                u.second->execute(0, 0, u.first, &hits[i], 0);
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
void SHLDSource::setInput(const std::string& filename, size_t length) { input = filename; }
