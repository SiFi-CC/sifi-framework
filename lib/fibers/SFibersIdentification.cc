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
    : fibOnlyAddress(nullptr), address(nullptr), fibData(nullptr)
{
    fibData = std::make_shared<identifiedFiberData>(); //needed?

}

 std::vector<std::shared_ptr<fibAddress>> SFibersIdentification::get4to1FiberFromSiPM(UInt_t SiPMID){
    
        fibOnlyAddress = std::make_shared<fibAddress>();
        std::vector<std::shared_ptr<fibAddress>> fibOnlyAddresses;
//         fibOnlyAddresses={nullptr};
        SFibersChannel* lc;
        SFibersLookupTable* pLookUp;
        pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("FibersPMILookupTable"));
        UInt_t fakeSiPMID = -1;
        

        for(int j = 0; j < n_fibers_per_SiPM; j++)
        {
            //get the fake ID
            fakeSiPMID=SiPMID*n_fibers_per_SiPM+j;
// //             address->realSiPMID=hits[i]->channelID;
// //             address->fakeSiPMID=((hits[i]->channelID)*n_fibers_per_SiPM+j);
// //             std::cout << address->realSiPMID << std::endl;
// //             std::cout << "fake" <<address->fakeSiPMID << std::endl;
            
            lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000, fakeSiPMID));
            fibOnlyAddress->mod=lc->m;
            fibOnlyAddress->lay=lc->l;
            fibOnlyAddress->fi=lc->s;
            fibOnlyAddress->side=lc->side;
//             std::cout << fibOnlyAddress->mod << " " << fibOnlyAddress->lay << " " << fibOnlyAddress->fi << " " << fibOnlyAddress->side << " " << std::endl;
            fibOnlyAddresses.push_back(fibOnlyAddress);
//             std::cout << fibOnlyAddresses[fibOnlyAddresses.size()-1]->mod << fibOnlyAddresses[fibOnlyAddresses.size()-1]->lay << fibOnlyAddresses[fibOnlyAddresses.size()-1]->fi << fibOnlyAddresses[fibOnlyAddresses.size()-1]->side << std::endl;
        }
//         for (int k = 0; k < fibOnlyAddresses.size(); k++){
// // //             std::cout << k << std::endl;
//             std::cout << fibOnlyAddresses[k]->mod << " " << fibOnlyAddresses[k]->lay << " " << fibOnlyAddresses[k]->fi << " " << fibOnlyAddresses[k]->side << " " << std::endl;
//         }
        
        return fibOnlyAddresses;
}

// UInt_t SFibersIdentification::get4to1SiPMFromFiber(std::vector<std::shared_ptr<fibAddress>> & fiber){
//     
// }


std::vector<std::shared_ptr<identifiedFiberData>> SFibersIdentification::identifyFibers(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    fibOnlyAddress = std::make_shared<fibAddress>();
    address = std::make_shared<fullAddress>();
    fibData = std::make_shared<identifiedFiberData>();
    std::vector<std::shared_ptr<fullAddress>> addresses;
    std::vector<std::shared_ptr<identifiedFiberData>> allFibData;
    int n_hits = hits.size();
//     int n_fibers_per_SiPM = 4;

    std::vector<std::shared_ptr<fibAddress>> fibOnlyAddresses;
    for (int i = 0; i < n_hits; i++)
    {
       //get a vector of fiber IDs from SiPM ID, write it to fibOnlyAddresses vector:
        fibOnlyAddresses=get4to1FiberFromSiPM(hits[i]->channelID);
       //access the contents of the fibOnlyAddresses vector:
//              for (int j = 0; j < fibOnlyAddresses.size(); j++){
//              std::cout << fibOnlyAddresses[j]->mod << " " << fibOnlyAddresses[j]->lay << " " << fibOnlyAddresses[j]->fi << " " << fibOnlyAddresses[j]->side << " " << std::endl;
//         }
        
     //   //do something with the obtained fibOnlyAddresses vector
        
        fibOnlyAddresses.clear();
        
        
        
/*//         SFibersChannel* lc;
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
//             address->lay=lc->l;
//             address->fi=lc->s;
//             address->side=lc->side;
//             addresses.push_back(address);
//         }
        //print whole addresses vector
        //addresses.clear();   */     
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
//         std::cout << "addad to allFibData" << std::endl;
    }
//     std::cout << std::endl;
    
return allFibData;    
}

