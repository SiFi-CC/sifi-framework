#include "STP4to1Source.h"
#include "SFibersIdentification.h"
#include "SUnpacker.h"
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
#include "SFibersLookup.h"

/**
 * Constructor. Requires subevent id for unpacked source and map of unpackers.
 *
 * \param subevent subevent id
 */
SFibersIdentification::SFibersIdentification()
    : address(nullptr), fibData(nullptr)
{
    fibData = std::make_shared<identifiedFiberData>();
}

std::vector<std::shared_ptr<identifiedFiberData>> SFibersIdentification::identifyFibers(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    address = std::make_shared<fullAddress>();
    fibData = std::make_shared<identifiedFiberData>();
    std::vector<std::shared_ptr<fullAddress>> addresses;
    std::vector<std::shared_ptr<identifiedFiberData>> allFibData;
    int n_hits = hits.size();
    int n_fibers_per_SiPM = 4;
    
//the code below can be used to retrieve the "fake address", which is probably useful for the identification algorithm
//     for (int i = 0; i < n_hits; i++)
//     {
//         SFibersChannel* lc;
// //         SLocator loc(3);
//         SFibersLookupTable* pLookUp;
//         pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersPMILookupTable"));
// 
//         for(int j = 0; j < n_fibers_per_SiPM; j++)
//         {
//             address->realSiPMID=hits[i]->channelID;
//             address->fakeSiPMID=((hits[i]->channelID)*n_fibers_per_SiPM+j);
//             std::cout << address->realSiPMID << std::endl;
//             std::cout << "fake" <<address->fakeSiPMID << std::endl;
//             lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, address->fakeSiPMID));
//             address->mod=lc->m;
//             address->layer=lc->l;
//             address->fiber=lc->s;
//             address->side=lc->side;
//             addresses.push_back(address);
//         }
//         
//         addresses.clear();        
//     }
    
    //change the code below by inserting the fiber identification algorithm and based on the algorithm, fill the allFibData structure for all subevents
    int n_fiber_hits = 5;
    for (int i = 0; i < n_fiber_hits; i++)
    {
        fibData->energyL=0.0;
        fibData->timeL=0.0;
        fibData->energyR=0.0;
        fibData->timeR=0.0;
        fibData->mod=0;
        fibData->lay=0;
        fibData->fi=0;
        allFibData.push_back(fibData);
    }
    
return allFibData;    
}

