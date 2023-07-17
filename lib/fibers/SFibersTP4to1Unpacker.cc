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
#include "SDatabase.h" // for pm, SDatabase
#include "SFibersLookup.h"
#include "SFibersRaw.h"
#include "SFibersCal.h"
#include "SFibersTOFPETCalibratorPar.h"
#include "SLocator.h" // for SLocator
#include "SLookup.h"  // for SLookupChannel, SLookupTable
//#include "SPMISource.h"
#include "STPSource.h"
#include "SFibersIdentification.h" //for identifiedFiberData structure. careful! is it a cross reference?
#include "SiFi.h"
#include "SFibersCalibrator.h"
#include "SCalContainer.h"

#include <TObject.h> // for TObject

#include <iostream>
#include <string> // for allocator

/**
 * Constructor
 */
SFibersTP4to1Unpacker::SFibersTP4to1Unpacker()
    : SUnpacker(), catFibersRaw(nullptr), catFibersCal(nullptr), pTOFPETCalPar(nullptr)
{
}

bool SFibersTP4to1Unpacker::init()
{
    SUnpacker::init();

    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersRaw);

    if (!catFibersRaw)
    {
        std::cerr << "No CatFibersRaw category" << std::endl;
        return false;
    }

    catFibersCal = sifi()->buildCategory(SCategory::CatFibersCal);

    if (!catFibersCal)
    {
        std::cerr << "No CatFibersCal category" << std::endl;
        return false;
    }

    // get TP calibrator parameters
    pTOFPETCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FibersTOFPETCalibratorPar"));

    if (!pTOFPETCalPar)
    {
        std::cerr << "Parameter container 'STPCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

bool SFibersTP4to1Unpacker::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                 void* buffer, size_t /*length*/)
{
    // getting buffer
    identifiedFiberData* fiber_hit = static_cast<identifiedFiberData*>(buffer);
    
//     fiber_hit->print();

    if (!fiber_hit) return false;

    SLocator loc(3);
    loc[0] = fiber_hit->mod; // mod;
    loc[1] = fiber_hit->lay; // lay;
    loc[2] = fiber_hit->fi; // fib;
    
    // setting category
    SFibersRaw* pRaw = dynamic_cast<SFibersRaw*>(catFibersRaw->getObject(loc));
    if (!pRaw)
    {
        pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
//         std::cout << "locator: " << loc[0] << " " << loc[1] << " " << loc[2] << std::endl;
        new (pRaw) SFibersRaw;
    }

    pRaw->setAddress(loc[0], loc[1], loc[2]);
    
    Int_t mod = 0;
    Int_t lay = 0;
    Int_t fib = 0;
    pRaw->getAddress(mod, lay, fib);

    SFibersChannel chan_l;
    chan_l.m = mod;
    chan_l.l = lay;
    chan_l.s = fib;
    chan_l.side = 'l';
    SFibersChannel chan_r = chan_l;
    chan_r.side = 'r';

    auto _cp_l = pTOFPETCalPar->getPar(&chan_l);
    auto _cp_r = pTOFPETCalPar->getPar(&chan_r);

    auto&& cp_l = *_cp_l;
    auto&& cp_r = *_cp_r;

    float p0, p1, p2, p3, corrEnergyL, corrEnergyR, corr511L, corr511R, rescEnergyL, rescEnergyR;
    rescEnergyL=0;
    rescEnergyR=0;
    corrEnergyL=0;
    corrEnergyR=0;
    corr511L=0;
    corr511R=0;
    p0=8.0;
    p1=1.04676;
    p2=1.02734;
    p3=0.31909;	
    if(cp_l[0]!=-100.0){
        corr511L = (p0*pow(p1, pow(cp_l[0], p2))+p3*cp_l[0]-p0); //non-linearity correction
        corr511R = (p0*pow(p1, pow(cp_r[0], p2))+p3*cp_r[0]-p0); //non-linearity correction
    }
    else {
        std::cout << "Error in SFibersTP4to1Unpacker: cp_l[0]=-100.0" << std::endl;
    }

    rescEnergyL = (fiber_hit->energyL)*511.0/corr511L;
    rescEnergyR = (fiber_hit->energyR)*511.0/corr511R;

//     pRaw->setQDCL(fiber_hit->energyL);
//     pRaw->setQDCR(fiber_hit->energyR);
    pRaw->setQDCL(rescEnergyL);
    pRaw->setTimeL(fiber_hit->timeL);
    pRaw->setQDCR(rescEnergyR);
    pRaw->setTimeR(fiber_hit->timeR);

    return true;
}
