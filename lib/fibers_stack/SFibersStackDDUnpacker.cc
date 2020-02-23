// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersStackDDUnpacker.h"
#include "SFibersStackRaw.h"
#include "SDDSamples.h"
#include "SParManager.h"
#include "SCategory.h"
#include "SiFi.h"

#include <iostream>

/** \class SFibersStackDDUnpacker
\ingroup lib_fibers_stack

A unpacker task.

\sa STask
*/

/** Constructor
 */
SFibersStackDDUnpacker::SFibersStackDDUnpacker(uint16_t address, uint8_t channel)
    : SDDUnpacker(address), channel(channel), catDDSamples(nullptr)
{
}

/** Destructor
 */
SFibersStackDDUnpacker::~SFibersStackDDUnpacker()
{
}

/** Init task
 * \sa STask::init()
 * \return success
 */
bool SFibersStackDDUnpacker::init()
{
    SDDUnpacker::init();

    size_t sim_sizes[3];
    sim_sizes[0] = 1;
    sim_sizes[1] = 1;
    sim_sizes[2] = 16;

    if (!sifi()->registerCategory(SCategory::CatDDSamples, "SDDSamples", 3, sim_sizes, false)) return false;

    catDDSamples = sifi()->buildCategory(SCategory::CatDDSamples);
    if (!catDDSamples)
    {
        std::cerr << "No CatDDSamples category" << "\n";
        return false;
    }

    return true;
}

bool SFibersStackDDUnpacker::decode(float * data, size_t length)
{
    SLocator loc(3);
    loc[0] = 0;//mod;
    loc[1] = 0;//lay;
    loc[2] = 1;//fib;

    SDDSamples * pSamples = (SDDSamples *) catDDSamples->getObject(loc);
    if (!pSamples)
    {
        pSamples = (SDDSamples *) catDDSamples->getSlot(loc);
        pSamples = new (pSamples) SDDSamples;
    }

    if (pSamples)
        pSamples->fillSamples(data, length);

    return true;
}

/** Execute task
 * \sa STask::execute()
 * \return success
 */
// bool SFibersStackDDUnpacker::execute()
// {
//     // get input here
// 
//     // determine data size
//     int size = 0;
// 
//     for (int i = 0; i < size; ++i)
//     {
//         // do something with the data
// 
//         // calculate address
//         Int_t mod = 0;
//         Int_t lay = 0;
//         Int_t fib = 0;
// 
//         // take these from lookup table?
//         Float_t u = 0;
//         Float_t y = 0;
// 
//         // take this from data
//         Float_t adc = 0;
// 
//         SLocator loc(3);
//         loc[0] = mod;
//         loc[1] = lay;
//         loc[2] = fib;
// 
//         
// 
//         pRaw->setAddress(mod, lay, fib);
//         pRaw->setU(u);
//         pRaw->setY(y);
//         pRaw->setADC(adc);
//     }
// 
//     return true;
// }

/** Finalize task
 * \sa STask::finalize()
 * \return success
 */
// bool SFibersStackDDUnpacker::finalize()
// {
//     return true;
// }
