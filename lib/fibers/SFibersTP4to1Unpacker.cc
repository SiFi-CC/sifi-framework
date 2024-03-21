// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersTP4to1Unpacker.h"
#include "SCategory.h"
#include "SLocator.h" // for SLocator
#include "STP4to1Source.h"
#include "SSiPMHit.h"
#include "SiFi.h"
#include "SCalContainer.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <cmath>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersTP4to1Unpacker::SFibersTP4to1Unpacker()
    : SUnpacker(), catSiPMHit(nullptr), pLookUp(nullptr)
{
}

bool SFibersTP4to1Unpacker::init()
{
    SUnpacker::init();

    catSiPMHit = sifi()->buildCategory(SCategory::CatSiPMHit);

    if (!catSiPMHit)
    {
        std::cerr << "No CatSiPMHit category" << std::endl;
        return false;
    }
    
    pLookUp = dynamic_cast<SSiPMsLookupTable*>(pm()->getLookupContainer("TPLookupTable"));
    
    return true;
}


float alignQDC(SSiPMHit *sipmData, float qdc){
    SCalContainer<6>* pSiPMCalPar;
    // get TP calibrator parameters
    pSiPMCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("SiPMsTOFPETCalibratorPar")); //TODO check if correct (previously getCalContainer("FibersTOFPETCalibratorPar") )

    if (!pSiPMCalPar)
    {
        std::cerr << "Parameter container 'SiPMsTOFPETCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
//     float SFibersRawClusterFinder::alignQDC(identifiedFiberDataC address, float qdc)
// {
    int mod,lay,el;
    char side;
    sipmData->getAddress(mod,lay,el,side);
    SFibersChannel chan;
    chan.m = mod;
    chan.l = lay;
    chan.s = el;
    chan.side = side;
    
    auto _cpar = pSiPMCalPar->getPar(&chan);
    auto&& cpar = *_cpar;
    
    if(cpar[0]==-100 || qdc<0)
    {
        //std::cout<<" Error: cpar[0]=-100.0" << std::endl;
        return -100;
    }
    else if(!std::isnan(qdc))
    {
        return qdc*511./cpar[0]; 
    }       
    else return -100;
}


bool SFibersTP4to1Unpacker::execute(ulong /*event*/, ulong seq_number, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{

    
    TP4to1Hit *hit = static_cast<TP4to1Hit*>(buffer);

    SLocator loc(1);
//     loc[0] = hits[i]->channelID; // changed for SiPM clustering!
//     loc[0] = i;  // new locator is hit ID
    loc[0] = seq_number; // new locator is hit ID
    SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMHit->getObject(loc));
    if (!pHit)
    {
        pHit = reinterpret_cast<SSiPMHit*>(catSiPMHit->getSlot(loc));
        new (pHit) SSiPMHit;
        if (!pHit)
        {
            std::cerr << "Error in STP4to1Extractor.cc: no pHit category!" << std::endl;
        }
    }
    SSiPMsChannel* lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hit->channelID));
    if(!lc)
    {
        fprintf(stderr, "STP4to1Extractor TOFPET2 absolute Ch%d missing. Check params.txt.\n", hit->channelID);
    } 
    else
    {
        if(lc->s==800 || lc->s==801){
            pHit->setChannel(lc->s);
            pHit->setAddress(lc->m, lc->l, lc->element, lc->side);
            pHit->setQDC(-100);
//             std::cout << lc->s << " " << lc->m << " " <<  lc->l << " " <<  lc->element << " " <<  lc->side <<  " hit->energy" << hit->energy << std::endl;
            pHit->setAlignedQDC(-100);
            pHit->setTime(-100);
//             pHit->setID(i);
            pHit->setID(seq_number);
        }
        else{
            pHit->setChannel(lc->s);
            pHit->setAddress(lc->m, lc->l, lc->element, lc->side);
            pHit->setQDC(hit->energy);
            pHit->setAlignedQDC(alignQDC(pHit, hit->energy));
            pHit->setTime(hit->time);
    //         pHit->setID(i);
            pHit->setID(seq_number);
        }

    }

    return true;
}
