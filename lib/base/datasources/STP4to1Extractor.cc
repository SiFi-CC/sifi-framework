#include "STP4to1Source.h"
#include "STP4to1Extractor.h"
#include "SFibersIdentification.h"
#include "SUnpacker.h"
#include "SFibersLookup.h"
#include "SLookup.h"
#include "SSiPMHit.h"
#include "SCategory.h"
#include "SiFi.h"
#include "SLocator.h"
#include <algorithm>          // for max, min
#include <cmath>              // for fabs
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for abort
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <map>
#include <utility> // for pair
#include <vector>  // for vector
#include <sstream>

#include"TFile.h"
#include"TTree.h"

/**
 * Constructor. Requires subevent id for unpacked source and map of unpackers.
 *
 * \param subevent subevent id
 */
STP4to1Extractor::STP4to1Extractor(uint16_t subevent, std::map<uint16_t, SUnpacker*> unpackers)
    : subevent(subevent), unpackers(unpackers)
{
}

STP4to1Extractor::~STP4to1Extractor()
{
    delete identification;
}


bool STP4to1Extractor::extract_open()
{
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

bool STP4to1Extractor::extract_close()
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

bool STP4to1Extractor::extract_readCurrentEvent(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    identification = new SFibersIdentification();
    std::vector fiberHits = identification->identifyFibers(hits);
   int n_fiber_hits = fiberHits.size();
    
    if (unpackers.size() == 0) return false;
    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        unpackers[subevent]->setSampleTimeBin(1.);
        unpackers[subevent]->setADCTomV(1.);
        for (int i = 0; i < n_fiber_hits; i++)
        {
            // TODO must pass event number to the execute
            unpackers[subevent]->execute(0, 0, subevent, fiberHits[i].get(), 0);
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
            u.second->setSampleTimeBin(1.);
            u.second->setADCTomV(1.);
            for (int i = 0; i < n_fiber_hits; i++)
            {
                u.second->execute(0, 0, u.first, fiberHits[i].get(), 0);
            }
        }
    }
return true;     
}

bool STP4to1Extractor::write_to_tree(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    // get SiPMHit category
    SCategory * catSiPMHit = sifi()->getCategory(SCategory::CatSiPMHit);
    if (!catSiPMHit)
    {
        std::cerr << "No CatSiPMHit category" << std::endl;
        return false;
    }

    SFibersLookupTable* pLookUp;
    pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("TPLookupTable"));
    int n_hits = hits.size();
    for (int i = 0; i < n_hits; i++)
    {
        SLocator loc(1);
        loc[0] = hits[i]->channelID;
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
        
        SFibersChannel* lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hits[i]->channelID) );
        if(!lc) {
            fprintf(stderr, "STP4to1Extractor TOFPET2 absolute Ch%d missing. Check params.txt.\n", hits[i]->channelID);
        } else {
            pHit->setChannel(lc->s);
            pHit->setAddress(lc->m, lc->l, lc->element, lc->side);
            pHit->setQDC(hits[i]->energy);
            pHit->setTime(hits[i]->time);
            
//             Float_t x{-100.}, y{-100.}, z{-100.};
//             if(lc->side=="l"){ //l means bottom
//                 //x = tab_of_x_coordinates_in_one_layer[lc->element] //check if it starts from 0
//                 //y = y_position_of_bottom_SIPM //to be changed for absorber!
//                 //z = z_offset[layer]
//             }
//             if(lc->side=="r"){ //r means top
//                 //x = tab_of_x_coordinates_in_one_layer[lc->element] +/- 1/2 SiPM Pitch shift!! //check if it starts from 0
//                 //y = y_position_of_top_SIPM //to be changed for absorber!
//                 //z = z_offset[layer]+/-1/2 SiPM Pitch shift!!
//             }
// //         Float_t rot = pGeomPar->getLayerRotation(loc[0], loc[1]);
// // 
// //         // calculate position from MLR
// // //        Float_t u = (log(sqrt(qdc_r / qdc_l)) - a0) * lambda;
// //         Float_t u = a_elar * log(sqrt(qdc_r / qdc_l)) + b_elar - pGeomPar->getFiberOffsetY(mod, lay); //calibration was done with fiber between 0 and 100mm
// //         Float_t s_u = 5.;
// // 
// //         // the fiber is taken from geometry
// //         Float_t v = pGeomPar->getFiberOffsetX(mod, lay) + fib * pGeomPar->getFibersPitch(mod, lay);
// //         Float_t s_v = 0.65;
// // 
// //         Float_t x = v * cos(rot * M_PI / 180) + u * sin(rot * M_PI / 180);
// //         Float_t y = v * sin(rot * M_PI / 180) + u * cos(rot * M_PI / 180);
// // 
// //         Float_t s_x = s_v * cos(rot * M_PI / 180) + s_u * sin(rot * M_PI / 180);
// //         Float_t s_y = s_v * sin(rot * M_PI / 180) + s_u * cos(rot * M_PI / 180);
// // 
// //         Float_t z = pGeomPar->getFiberOffsetZ(mod, lay) + pGeomPar->getModuleZ(mod);
// //         Float_t s_z = 0.65;
// 
// 
//             
//             
//             pHit->setXYZ(x, y, z);
        }
    }
 
return 0;    
}
