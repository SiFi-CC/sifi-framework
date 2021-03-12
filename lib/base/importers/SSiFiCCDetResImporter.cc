// @(#)lib/fibers:$Id$
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
#include "SFibersCalSim.h"
#include "SFibersGeomPar.h"
#include "SGeantFibersRaw.h"
#include "SGeantTrack.h"
#include "SParManager.h"
#include "SiFi.h"

#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>

const Float_t adc_to_mv = 4.096;
const Float_t sample_to_ns = 1.;
/**
 * \class SSiFiCCDetResImporter
\ingroup lib_fibers

A unpacker task.

\sa STask
*/

/**
 * Constructor
 */
SSiFiCCDetResImporter::SSiFiCCDetResImporter() : SUnpacker() {}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SSiFiCCDetResImporter::init()
{
    SUnpacker::init();

    catGeantTrack = sifi()->buildCategory(SCategory::CatGeantTrack);
    if (!catGeantTrack)
    {
        std::cerr << "No CatGeantTrack category" << std::endl;
        return false;
    }

    catGeantFibersRaw = sifi()->buildCategory(SCategory::CatGeantFibersRaw);
    if (!catGeantFibersRaw)
    {
        std::cerr << "No CatGeantFibersRaw category" << std::endl;
        return false;
    }

    catFibersCal = sifi()->buildCategory(SCategory::CatFibersCal);
    if (!catFibersCal)
    {
        std::cerr << "No CatFibersCal category" << std::endl;
        return false;
    }

    pGeomPar =
        dynamic_cast<SFibersGeomPar*>(pm()->getParameterContainer("FibersGeomPar"));
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'FibersGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

bool SSiFiCCDetResImporter::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                    void* buffer, size_t /*length*/)
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
    Float_t pos_z = tree->pos.Z();

    Float_t rot = pGeomPar->getLayerRotation(loc[0], loc[1]);
    Float_t u = pos_x * cos(rot * M_PI / 180) + pos_y * sin(rot * M_PI / 180);
    Float_t v = pos_x * sin(rot * M_PI / 180) + pos_y * cos(rot * M_PI / 180);

    for (auto t : tree->kine)
    {
        if (t.dir.Mag() == 0) continue;

        SGeantTrack* pTrack = reinterpret_cast<SGeantTrack*>(catGeantTrack->getNewSlot());
        pTrack = new (pTrack) SGeantTrack;
        TVector3 v_p = t.dir;
        v_p.SetMag(t.E);
        pTrack->SetVectM(v_p, 0);
        pTrack->setStartXYZ(t.pos);
        pTrack->setType(t.type);
    }

    SFibersCalSim* pCal = dynamic_cast<SFibersCalSim*>(catFibersCal->getObject(loc));
    if (!pCal)
    {
        pCal = reinterpret_cast<SFibersCalSim*>(catFibersCal->getSlot(loc));
        pCal = new (pCal) SFibersCalSim;
    }

    pCal->setAddress(loc[0], loc[1], loc[2]);
    if (side == 'l' and pCal->getQDCL() == 0.0)
    {
        pCal->setGeantPoint({u, v, pos_z});
        pCal->setQDCL(tree->data.counts);
        pCal->setTimeL(tree->data.time);
        pCal->setGeantEnergyLoss(tree->energy_dep);
    }
    if (side == 'r' and pCal->getQDCR() == 0.0)
    {
        pCal->setGeantPoint({u, v, pos_z});
        pCal->setQDCR(tree->data.counts);
        pCal->setTimeR(tree->data.time);
        pCal->setGeantEnergyLoss(tree->energy_dep);
    }

    return true;
}
