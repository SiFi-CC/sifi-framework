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
#include "SCategory.h"
#include "SSimSource.h"
#include "SDatabase.h"
#include "SFibersCalSim.h"
#include "SFibersGeomPar.h"
#include "SDetectorGeomPar.h"
#include "SGeantTrack.h"
#include "SLocator.h" // for SLocator
#include "SPar.h"     // for SPar
#include "SiFi.h"
#include "SSiPMHit.h"
#include "SFibersLookup.h"
#include "SCalContainer.h"
#include "SEventNumber.h"

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
        std::cerr << "Parameter container 'DetectorGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

bool SSimImporter::execute(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                    void* buffer, size_t /*length*/)
{
    return true;
}

bool SSimImporter::execute_withEntryID(ulong /*event*/, ulong /*seq_number*/, uint16_t /*subevent*/,
                                    void* buffer, size_t /*length*/, long entryID)
{
    std::vector<SiPMData>* vSiPMData = static_cast<std::vector<SiPMData>*>(buffer);
    if (!vSiPMData) return false;
    
    Int_t eID = entryID;
    Int_t nHits = vSiPMData->size();
    SiPMData currentHit;

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
            if (!pHit)
            {
                std::cerr << "Error in SSimImporter.cc: no pHit category!" << std::endl;
            }
        }


        Int_t id = currentHit.fSiPMId;
        Int_t module;
        Int_t layer;
        Int_t element;
        char side;
        if(pDetGeomPar->getAddressFromswSiPMID(id, module, layer, element, side) == false)
        {
            fprintf(stderr, "Error in SSimImporter.cc: swSiPMID %d missing. Check params.txt.\n", currentHit.fSiPMId);
        }
        else
        {
            pHit->setChannel(id);
            pHit->setAddress(module, layer, element, side);
            pHit->setQDC(currentHit.fSiPMPhotonCount);
//             pHit->setAlignedQDC(alignQDC(pHit, currentHit.fSiPMPhotonCount));
            pHit->setAlignedQDC(currentHit.fSiPMPhotonCount); //no alignment in simulation => QDC=alignedQDC
            pHit->setTime(currentHit.fSiPMTimeStamp);
            pHit->setID(i);
        }
    }
//     std::cout << std::endl;
    return true;
}
