#include "SFibersIdentification.h"
#include "SCategory.h"
#include "SLocator.h"
#include "SLookup.h"
#include "SSiPMHit.h"
#include "STP4to1Source.h"
#include "SUnpacker.h"
#include "STask.h"
#include "SiFi.h"
#include "SFibersLookup.h"
#include <TFile.h>
#include <TTree.h>
#include <algorithm>          // for max, min
#include <cmath>              // for fabs
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for abort
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <map>
#include <sstream>
#include <utility> // for pair
#include <vector>  // for vector

/**
 * Constructor. Requires ...
 *
 * \param subevent subevent id
 */

SFibersIdentification::SFibersIdentification() : STask(), catSiPMsHit(nullptr), catSiPMsCluster(nullptr), catFibersRaw(nullptr), fibLookup(nullptr)
{}

bool SFibersIdentification::init()
{ 
    
    catSiPMsHit = sifi()->getCategory(SCategory::CatSiPMHit);
    
    if(!catSiPMsHit)
    {
        std::cerr << "No CatSiPMHit category!" << std::endl;
        return false;
    }
    
    catSiPMsHit->print(); // BUG dlaczego print pokazuje pustą kategorię?
    
    catSiPMsCluster = sifi()->getCategory(SCategory::CatSiPMClus);
   
    if(!catSiPMsCluster)
    {
       std::cerr << "No CatSiPMClus category!" << std::endl;
    }
    catSiPMsCluster->print(); // BUG dlaczego print pokazuje pustą kategorię?
    
    catFibersRaw = sifi()->buildCategory(SCategory::CatFibersRaw);
    
    if(!catFibersRaw)
    {
        std::cerr << "No CatFibersRaw category!" << std::endl;
        return false;
    }
    
    fibLookup = dynamic_cast<SMultiFibersLookupTable*>(pm()->getLookupContainer("4to1SiPMtoFibersLookupTable"));
    
    return true;
}

bool SFibersIdentification::execute()
{ 
    
    SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(0));
    pHit->print();
    
    std::cout << __func__ << " entries in hit: " << catSiPMsHit->getEntries() << std::endl;
    
    if(catSiPMsCluster->getEntries() > 0)
    {
        SSiPMCluster* pClus = dynamic_cast<SSiPMCluster*>(catSiPMsCluster->getObject(0));
        std::cout << __func__ << " entries in clus: " << catSiPMsCluster->getEntries() << std::endl;
    
        std::vector<fibAddress> f = getFibersFromCluster(pClus);
        
        for(auto i : f)
            i.print();
    }
    
    return true;
}

bool SFibersIdentification::finalize() { return true; }

// std::vector<std::shared_ptr<fibAddress>> SFibersIdentification::get4to1FiberFromSiPM(UInt_t SiPMID)
// {
// 
//     std::vector<std::shared_ptr<fibAddress>> fibOnlyAddresses;
// 
//     return fibOnlyAddresses;
// }
// 
// UInt_t SFibersIdentification::get4to1SiPMFromFiber(std::vector<std::shared_ptr<fibAddress>>& fiber)
// {
// 
//     return true;
// }

fibAddress convertAddress(std::vector<std::string> v)
{
    if(v.size() != 4)
    {
        std::cout << "In " << __func__ << std::endl;
        std::cout << "Vector length should be 4!" << std::endl;
        std::abort();
    }
    
    fibAddress f;
    f.mod = stoi(v[0]);
    f.lay = stoi(v[1]);
    f.fi = stoi(v[2]);
    f.side = v[3][0];
    
    return f;
}

std::vector <fibAddress> SFibersIdentification::getFibersFromCluster(SSiPMCluster *cluster)
{
    
//     std::cout << __func__ << std::endl;
    
    std::vector <Int_t> hits = cluster->getHitsArray();
    int nhits = hits.size();
    
    SMultiFibersChannel* fibLookupChannel;
    
    SSiPMHit* pHit = nullptr;
    int chan = 0;
    
    std::vector <fibAddress> addresses;
    std::vector<std::vector<std::string>> vec;
    std::vector<std::string> tmp_str_addr;
    
    for(int i=0; i<nhits; ++i)
    {
        pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(hits[i]));
        pHit->getChannel(chan);
        fibLookupChannel = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, chan));
        vec = fibLookupChannel->vecFiberAssociations;
        
//         std::cout << "Printing from SMulitFibersLookupTable:" << std::endl;
        
        for(int j=0; j<vec.size(); ++j)
        {
            for(int k=0; k<vec[j].size(); ++k)
            {
//                 std::cout << vec[j][k] << std::endl;
                tmp_str_addr.push_back(vec[j][k]);
            }
            
            fibAddress tmp_fib_addr = convertAddress(tmp_str_addr);
            addresses.push_back(tmp_fib_addr);
//             tmp_fib_addr.print();
            tmp_str_addr.clear();
        }
        
    }
    
    return addresses;
}

// std::vector<std::shared_ptr<identifiedFiberData>>
// SFibersIdentification::identifyFibers(std::vector<std::shared_ptr<TP4to1Hit>>& hits)
// {

/*
        std::vector<std::vector<UInt_t>> scat_bottom;
        std::vector<std::vector<UInt_t>> ab_bottom;
        std::vector<std::vector<UInt_t>> scat_top;
        std::vector<std::vector<UInt_t>> ab_top;
        if (clusters_final.size())
        {

            for (int i = 0; i < clusters_final.size(); i++)
            {
                lc = dynamic_cast<SSiPMsChannel*>(
                    pLookUp->getAddress(0x1000, hits[clusters_final[i][0]]->channelID));
                if (lc->m == 0 and lc->side == 'l') { scat_bottom.push_back(clusters_final[i]); }
                if (lc->m == 1 and lc->side == 'l') { ab_bottom.push_back(clusters_final[i]); }
                if (lc->m == 0 and lc->side == 'r') { scat_top.push_back(clusters_final[i]); }
                if (lc->m == 1 and lc->side == 'r') { ab_top.push_back(clusters_final[i]); }
            }

            SMultiFibersChannel* lc2;
            SMultiFibersLookupTable* pLookUp2;
            pLookUp2 = dynamic_cast<SMultiFibersLookupTable*>(
                pm()->getLookupContainer("4to1SiPMtoFibersLookupTable"));
            std::vector<std::vector<std::string>> vec;
            std::vector<std::vector<std::vector<std::string>>> cluster_fibers_sb;
            std::vector<std::vector<std::vector<std::string>>> cluster_fibers_st;
            std::vector<std::vector<std::string>> SiPMfibers;

            if (scat_bottom.size())
            {
                for (int i = 0; i < scat_bottom.size(); i++)
                {
                    for (int j : scat_bottom[i])
                    {
                        lc = dynamic_cast<SSiPMsChannel*>(
                            pLookUp->getAddress(0x1000, hits[j]->channelID));
                        // lc2=
                        // dynamic_cast<SMultiFibersChannel*>(pLookUp->getAddress(0x1000,lc->s));
                        lc2 =
                            dynamic_cast<SMultiFibersChannel*>(pLookUp2->getAddress(0x1000, lc->s));
                        if (lc->s == 800) continue;
                        vec = lc2->vecFiberAssociations;

                        // std::cout<<"SiPMId: "<<lc->s<<std::endl;
                        // std::cout<<"vec size: "<<vec.size()<<std::endl;
                        for (std::vector<std::string> k : vec)
                        {
                            SiPMfibers.push_back(k);
                        }
                    }
                    cluster_fibers_sb.push_back(SiPMfibers);
                    SiPMfibers.clear();
                }
            }

            for (int i = 0; i < scat_top.size(); i++)
            {
                for (int j : scat_top[i])
                {
                    lc = dynamic_cast<SSiPMsChannel*>(
                        pLookUp->getAddress(0x1000, hits[j]->channelID));
                    lc2 = dynamic_cast<SMultiFibersChannel*>(pLookUp2->getAddress(0x1000, lc->s));
                    if (lc->s == 800) continue;
                    vec = lc2->vecFiberAssociations;
                    for (std::vector<std::string> k : vec)
                    {
                        SiPMfibers.push_back(k);
                    }
                }
                cluster_fibers_st.push_back(SiPMfibers);
                SiPMfibers.clear();
            }

            std::vector<std::vector<std::string>> in_fiber;
            std::vector<std::vector<std::vector<std::string>>> cluster_in_fibers;
            if (cluster_fibers_sb.size() and cluster_fibers_st.size())
            {
                for (int i = 0; i < cluster_fibers_sb.size(); i++)
                {
                    for (int j = 0; j < cluster_fibers_st.size(); j++)
                    {
                        for (std::vector<std::string> fibers_bottom : cluster_fibers_sb[i])
                        {
                            for (std::vector<std::string> fibers_top : cluster_fibers_st[j])
                            {
                                if (fibers_bottom[0] == fibers_top[0] and
                                    fibers_bottom[1] == fibers_top[1] and
                                    fibers_bottom[2] == fibers_top[2])
                                {
                                    fibers_bottom.push_back(std::to_string(i));
                                    fibers_bottom.push_back(std::to_string(j));
                                    in_fiber.push_back(fibers_bottom);
                                    // std::cout<<fibers_bottom[0]<<" "<<fibers_bottom[1]<<"
                                    // "<<fibers_bottom[2]<<" "<<fibers_bottom[3]<<"
                                    // "<<fibers_bottom[4]<<" "<<fibers_bottom[5]<<std::endl;
                                }
                            }
                        }
                        cluster_in_fibers.push_back(in_fiber);
                        in_fiber.clear();
                    }
                }
            }

            float qdc_l = 0;
            long long int time_l = 0;
            float qdc_r = 0;
            long long int time_r = 0;

            for (std::vector<std::vector<std::string>> k : cluster_in_fibers)
            {
                if (k.size() == 1)
                {
                    for (int i : scat_bottom[std::stoi(k[0][4])])
                    {
//                         std::cout << i << " " << hits.size() << std::endl;
                        qdc_l = qdc_l + hits[i]->energy;
                        time_l = hits[i]->time;
                    }
                    for (int i : scat_top[std::stoi(k[0][5])])
                    {
//                         std::cout << i << " " << hits.size() << std::endl;
                        qdc_r = qdc_r + hits[i]->energy;
                        time_r = hits[i]->time;
                    }
                    fibData->energyL = qdc_l;
                    fibData->timeL = time_l;
                    fibData->energyR = qdc_r;
                    fibData->timeR = time_r;
                    fibData->mod = std::stoi(k[0][0]);
                    fibData->lay = std::stoi(k[0][1]);
                    fibData->fi = std::stoi(k[0][2]);
                    allFibData.push_back(fibData);
                }
            }

            //
            //    //change the code below by inserting the fiber identification algorithm and based
            //    on the algorithm, fill the allFibData structure for all subevents int n_subevents
            //    = 5; //number of subevents for (int i = 0; i < n_subevents; i++)
            //    {
            //        fibData->energyL=0.0;
            //        fibData->timeL=0.0;
            //        fibData->energyR=0.0;
            //        fibData->timeR=0.0;
            //        fibData->mod=0;
            //        fibData->lay=0;
            //        fibData->fi=0;
            //        allFibData.push_back(fibData);
            //    }
            SiPMadresses.clear();
            candidates.clear();
            clusters.clear();
            clusters_final.clear();
        }
    }

    //     for (auto a : allFibData)
    //     {
    //         a->print();
    //     }

    return allFibData;*/
// }
