// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SSimImporter.h"
#include "SCalContainer.h"
#include "SCategory.h"
#include "SDatabase.h"
#include "SDetectorGeomPar.h"
#include "SEventNumber.h"
#include "SFibersCalSim.h"
#include "SFibersGeomPar.h"
#include "SFibersLookup.h"
#include "SGeantTrack.h"
#include "SLocator.h" // for SLocator
#include "SPar.h"     // for SPar
#include "SSiPMHit.h"
#include "SSimSource.h"
#include "SiFi.h"

#include <RtypesCore.h> // for Float_t
#include <TObject.h>    // for TObject
#include <TVector3.h>   // for TVector3

#include <cstdlib> // for exit, EXIT_FAILURE, size_t
#include <iostream>
#include <math.h>
#include <vector> // for vector

/**
 * \class SSimImporter
\ingroup lib_fibers

An unpacker task.

\sa STask
*/

/**
 * Constructor
 */
SSimImporter::SSimImporter() : SUnpacker() {}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */

bool SSimImporter::init()
{
    SUnpacker::init();

    catSiPMHit = sifi()->buildCategory(SCategory::CatSiPMHit);
    if (!catSiPMHit)
    {
        std::cerr << "No CatSiPMHit category" << std::endl;
        return false;
    }

    catEventNumber = sifi()->buildCategory(SCategory::CatEventNumber);
    if (!catEventNumber)
    {
        std::cerr << "No CatEventNumber category" << std::endl;
        return false;
    }

    pDetGeomPar = dynamic_cast<SDetectorGeomPar*>(pm()->getParContainer("DetectorGeomPar"));
    if (!pDetGeomPar)
    {
        // Geometry is only needed to translate the compact SiPM IDs stored in the
        // simulation into framework addresses. Without it we can still import the
        // hits (channel/QDC/time) and leave the address unset, which is enough for
        // minimal I/O runs (see dst_4to1_sim --no_tasks). Warn instead of aborting
        // so that such runs do not die here.
        std::cerr
            << "Parameter container 'DetectorGeomPar' was not obtained; continuing without geometry mapping."
            << std::endl;
    }

    return true;
}

bool SSimImporter::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                           void* buffer, size_t /*length*/)
{
    return true;
}

bool SSimImporter::execute_withEntryID(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                       void* buffer, size_t /*length*/, Long64_t entryID)
{
    // The buffer is the SiPMData vector filled by SSimSource from the current
    // TChain entry. entryID is the index of that entry in the chain; it is stored
    // in CatEventNumber so downstream tasks can correlate output with input even
    // when the EventNumber branch inside the file is not contiguous.
    std::vector<SiPMData>* vSiPMData = static_cast<std::vector<SiPMData>*>(buffer);
    if (!vSiPMData) return false;

    // CatEventNumber stores an Int_t, so narrow the ROOT entry index here.
    Int_t eID = static_cast<Int_t>(entryID);
    Int_t nHits = vSiPMData->size();
    SiPMData currentHit;

    // One SEventNumber object per event, placed at a fixed locator.
    SLocator en_loc(3);
    SEventNumber* pEventNumber = dynamic_cast<SEventNumber*>(catEventNumber->getObject(en_loc));
    if (!pEventNumber)
    {
        pEventNumber = reinterpret_cast<SEventNumber*>(catEventNumber->getSlot(en_loc));
        new (pEventNumber) SEventNumber;
        if (!pEventNumber)
        {
            std::cerr << "Error in SSimImporter.cc: no pEventNumber category!" << std::endl;
        }
    }
    pEventNumber->setEventNumber(eID);
    //     std::cout << "eventID: " << eID << std::endl;

    for (Int_t i = 0; i < nHits; ++i)
    {
        currentHit = vSiPMData->at(i);
        SLocator loc(1);
        loc[0] = i; // locator is hit ID
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMHit->getObject(loc));
        if (!pHit)
        {
            pHit = reinterpret_cast<SSiPMHit*>(catSiPMHit->getSlot(loc));
            new (pHit) SSiPMHit;
            if (!pHit) { std::cerr << "Error in SSimImporter.cc: no pHit category!" << std::endl; }
        }

        const Int_t rawId = currentHit.fSiPMId;

        // The simulation files used in this project store fSiPMId in a compact range
        // that does not encode the readout side. The side is, however, encoded in the
        // position (typically Y = +/- 51 mm). If we have DetectorGeomPar available,
        // convert (module, layer, element, side) -> framework swSiPMID.
        //
        // Steps:
        //   1. decode the raw ID into (module, layer, element) - the side it yields
        //      is meaningless for these files and is deliberately ignored;
        //   2. derive the side from the hit's Y position (right = +Y, left = -Y);
        //   3. re-encode the full address, now including the correct side, into the
        //      framework swSiPMID that the rest of the reconstruction expects.
        // If the raw ID is unknown or the position is ambiguous we keep rawId as-is
        // and let the address lookup below report the problem.
        Int_t swId = rawId;
        if (pDetGeomPar)
        {
            Int_t module = -1;
            Int_t layer = -1;
            Int_t element = -1;
            char sideFromId = '?';

            if (pDetGeomPar->getAddressFromswSiPMID(rawId, module, layer, element, sideFromId))
            {
                const double y = currentHit.fSiPMPosition.Y();
                char sideFromPos = '?';
                if (fabs(y) > 1e-6) { sideFromPos = (y > 0.0) ? 'r' : 'l'; }

                if (sideFromPos == 'l' || sideFromPos == 'r')
                {
                    pDetGeomPar->getswSiPMIDFromAddress(swId, module, layer, element, sideFromPos);
                }
            }
        }

        pHit->setChannel(swId);
        pHit->setQDC(currentHit.fSiPMPhotonCount);
        // Simulation has no gain alignment, so the aligned QDC equals the raw one.
        pHit->setAlignedQDC(currentHit.fSiPMPhotonCount);
        pHit->setTime(currentHit.fSiPMTimeStamp);
        pHit->setID(i);

        // Translate the (possibly corrected) swSiPMID into a detector address.
        // Without geometry we cannot do this, so mark the address as invalid
        // rather than leaving stale values in the hit.
        if (pDetGeomPar)
        {
            Int_t module;
            Int_t layer;
            Int_t element;
            char side;
            if (pDetGeomPar->getAddressFromswSiPMID(swId, module, layer, element, side) == false)
            {
                fprintf(stderr, "Error in SSimImporter.cc: swSiPMID %d missing. Check params.txt.\n", swId);
                pHit->setAddress(-1, -1, -1, '?');
            }
            else
            {
                pHit->setAddress(module, layer, element, side);
            }
        }
        else
        {
            pHit->setAddress(-1, -1, -1, '?');
        }
    }
    //     std::cout << std::endl;
    return true;
}
