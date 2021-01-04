// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SSiFiCCDetResImporter.h"
#include "SCategory.h"
#include "SFibersStackCal.h"
#include "SFibersStackCalSim.h"
#include "SFibersStackGeomPar.h"
#include "SParManager.h"
#include "SiFi.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h>

const Float_t adc_to_mv = 4.096;
const Float_t sample_to_ns = 1.;
/**
 * \class SSiFiCCDetResImporter
\ingroup lib_fibers_stack

A unpacker task.

\sa STask
*/

/**
 * Constructor
 */
SSiFiCCDetResImporter::SSiFiCCDetResImporter() : SUnpacker()
{
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SSiFiCCDetResImporter::init()
{
    SUnpacker::init();

    catFibersCal = sifi()->buildCategory(SCategory::CatFibersStackCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersStackCal category"
                  << "\n";
        return false;
    }

    pGeomPar = dynamic_cast<SFibersStackGeomPar*>(pm()->getParameterContainer("SFibersStackGeomPar"));
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'SFibersStackGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

bool SSiFiCCDetResImporter::execute(ulong /*event*/, ulong /*seq_number*/,
                                    uint16_t /*subevent*/, void* buffer,
                                    size_t /*length*/)
{
    TREE_all* tree = static_cast<TREE_all*>(buffer);
    if (!tree) return false;

    SLocator loc(3);
    loc[0] = tree->address.m; // mod;
    loc[1] = tree->address.l; // lay;
    loc[2] = tree->address.f; // fib;
    char side = tree->address.s;

    Float_t pos_x = tree->pos.X();
    Float_t pos_y = tree->pos.Y();

    Float_t rot = pGeomPar->getLayerRotation(loc[0], loc[1]);
    Float_t u = pos_x * cos(rot * M_PI/180) + pos_y * sin(rot * M_PI/180);

    SFibersStackCalSim* pCal = dynamic_cast<SFibersStackCalSim*>(catFibersCal->getObject(loc));
    if (!pCal)
    {
        pCal = reinterpret_cast<SFibersStackCalSim*>(catFibersCal->getSlot(loc));
        pCal = new (pCal) SFibersStackCalSim;
    }

    pCal->setAddress(loc[0], loc[1], loc[2]);
    if (side == 'l' and pCal->getQDCL() == 0.0) {
        pCal->setGeantU(u);
        pCal->setQDCL(tree->data.counts);
        pCal->setTimeL(tree->data.time);
    }
    if (side == 'r' and pCal->getQDCR() == 0.0) {
        pCal->setGeantU(u);
        pCal->setQDCR(tree->data.counts);
        pCal->setTimeR(tree->data.time);
    }

    return true;
}
