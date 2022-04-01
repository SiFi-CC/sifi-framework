// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "STTreeSource.h"
#include "SUnpacker.h"

#include <iostream>
#include <sstream>
#include <map>

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
STTreeSource::STTreeSource(uint16_t subevent)
    : SDataSource(), subevent(subevent), input(), istream()
{
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool STTreeSource::open()
{
    TFile *file = new TFile(input.c_str(), "READ");
    if (file->IsZombie()) {
        std::cerr << "##### Error in " << __PRETTY_FUNCTION__ << "! Could not open input file!" << std::endl;
        std::cerr << input << std::endl;
        return false;
    }
    tree = (TTree *) file->Get("FiberCoincidences");
    tree->SetBranchAddress("GlobalFiberNumber", &GlobalFiberNumber);
    tree->SetBranchAddress("PhotonsRoiR", &PhotonsRoiR);
    tree->SetBranchAddress("PhotonsRoiL", &PhotonsRoiL);
    tree->SetBranchAddress("TimeStampR", &TimeStampR);
    tree->SetBranchAddress("TimeStampL", &TimeStampL);

    /**
     * get acquisition T_0
     */
    tree->GetEntry(0);
    acqT0L = TimeStampL;
    acqT0R = TimeStampR;

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

bool STTreeSource::close()
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
    delete tree;
    return true;
}

bool STTreeSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;
    std::vector<TTreeHit> hits;
    if(getCurrentEvent() > tree->GetEntries() ) {
        std::cerr << "##### Error in " << __PRETTY_FUNCTION__ << "! Invalid TTree entry" << std::endl;
        return false;
    }
    tree->GetEntry(getCurrentEvent() );
    TTreeHit hit_cache;
    //I think this timestamp is since the beginning of the acquisition
    hit_cache.time_l = 1e-3 * (TimeStampL - acqT0L); //ps to ns
    hit_cache.time_r = 1e-3 * (TimeStampR - acqT0R);
    //photon numbers, analogous to QDC
    hit_cache.qdc_l = PhotonsRoiL;
    hit_cache.qdc_r = PhotonsRoiR;
    //this is after fiber identification
    hit_cache.fiberID = GlobalFiberNumber;
    hits.push_back(hit_cache);

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
void STTreeSource::setInput(const std::string& filename, size_t length) { input = filename; }
