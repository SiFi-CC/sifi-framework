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
 * Constructor. Requires ...
 *
 * \param subevent subevent id
 */
SFibersIdentification::SFibersIdentification()
    : STask(), fibOnlyAddress(nullptr), fibData(nullptr)
{
    fibOnlyAddress = std::make_shared<fibAddress>();
    fibData = std::make_shared<identifiedFiberData>();
}

bool SFibersIdentification::init()
{
    return true;
}


bool SFibersIdentification::execute()
{
    return true;
}

bool SFibersIdentification::finalize()
{
    return true;
}

 std::vector<std::shared_ptr<fibAddress>> SFibersIdentification::get4to1FiberFromSiPM(UInt_t SiPMID){
    
        std::vector<std::shared_ptr<fibAddress>> fibOnlyAddresses;
        SFibersChannel* lc;
        SFibersLookupTable* pLookUp;
        pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersPMILookupTable"));
        UInt_t fakeSiPMID = -1;
        
        for(int j = 0; j < n_fibers_per_SiPM; j++)
        {
            fakeSiPMID=SiPMID*n_fibers_per_SiPM+j;
            lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, SiPMID));
            if(!lc) {
                printf("TOFPET2 Ch%d missing. Check params.txt.\n", SiPMID);
            } else {
                fibOnlyAddress->mod=lc->m;
                fibOnlyAddress->lay=lc->l;
                fibOnlyAddress->fi=lc->s;
                fibOnlyAddress->side=lc->side;
                fibOnlyAddresses.push_back(fibOnlyAddress);
            }
        }        
        return fibOnlyAddresses;
}

UInt_t SFibersIdentification::get4to1SiPMFromFiber(std::vector<std::shared_ptr<fibAddress>> & fiber){
   //to be implemented later 
    return true;
}


std::vector<std::shared_ptr<identifiedFiberData>> SFibersIdentification::identifyFibers(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    std::vector<std::shared_ptr<identifiedFiberData>> allFibData;
    int n_hits = hits.size();

    std::vector<std::shared_ptr<fibAddress>> fibOnlyAddresses;
    for (int i = 0; i < n_hits; i++) //
    {
       //get a vector of fiber IDs from SiPM ID, write it to fibOnlyAddresses vector:
        fibOnlyAddresses=get4to1FiberFromSiPM(hits[i]->channelID);
       //access the contents of the fibOnlyAddresses vector:
//              for (int j = 0; j < fibOnlyAddresses.size(); j++){
//              std::cout << fibOnlyAddresses[j]->mod << " " << fibOnlyAddresses[j]->lay << " " << fibOnlyAddresses[j]->fi << " " << fibOnlyAddresses[j]->side << " " << std::endl;
//         }
        
     //   //do something with the obtained fibOnlyAddresses vector
        fibOnlyAddresses.clear();
    }
    
    //change the code below by inserting the fiber identification algorithm and based on the algorithm, fill the allFibData structure for all subevents
    int n_subevents = 5; //number of subevents
    for (int i = 0; i < n_subevents; i++)
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

