#include "STP4to1Source.h"
#include "SSiPMsGeomPar.h"
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
#include<SFibersLookup.h>
#include"TFile.h"
#include"TTree.h"
#include "SMultiFibersLookup.h"

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
    
        return fibOnlyAddresses;
}

UInt_t SFibersIdentification::get4to1SiPMFromFiber(std::vector<std::shared_ptr<fibAddress>> & fiber){

    return true;
}


std::vector<std::shared_ptr<identifiedFiberData>> SFibersIdentification::identifyFibers(std::vector<std::shared_ptr<TP4to1Hit>> & hits)
{
    std::vector<std::shared_ptr<identifiedFiberData>> allFibData;
    int n_hits = hits.size();

       std::vector<std::vector<UInt_t>> SiPMadresses;
       std::vector<UInt_t> ja;
 
        SFibersChannel* lc;
        SFibersLookupTable* pLookUp;
        pLookUp = dynamic_cast<SFibersLookupTable*>(pm()->getLookupContainer("TPLookupTable"));

        for(int j = 0; j <n_hits; j++)
        {

                lc = dynamic_cast<SFibersChannel*>(pLookUp->getAddress(0x1000,hits[j]->channelID ));

                
                ja.push_back(j);
                ja.push_back(lc->m);
                ja.push_back(lc->l);
                ja.push_back(lc->element);
                

                if(lc->side=='r'){
                    ja.push_back(0);
                    
                }
                if(lc->side=='l'){
                    ja.push_back(1);
                    
                }
                SiPMadresses.push_back(ja);
                ja.clear();
            
        }
//std::cout<<"new event"<<std::endl;
 // algorithm for clustering- ugly version          
           std::vector<std::vector<UInt_t>> cluster;
           std::vector<UInt_t> cl;
           int not_cluster=1;
           std::vector<std::vector<std::vector<UInt_t>>> clusters;
           std::vector<std::vector<UInt_t>> candidates;
           std::vector<int> couter;
           int cluster_size;
           int a=0;
           /*for(int i=0; i<SiPMadresses.size();i++)
                {
                    candidates.push_back(SiPMadresses[i]);
                }*/
            while(not_cluster){
                candidates.clear();
                couter.clear();
                not_cluster=0;
                cluster.clear();
                cl.clear();
                if(!SiPMadresses.empty()){
                    cl.push_back(SiPMadresses[0][0]);
                    cl.push_back(SiPMadresses[0][1]);
                    cl.push_back(SiPMadresses[0][2]);
                    cl.push_back(SiPMadresses[0][3]);
                    cl.push_back(SiPMadresses[0][4]);
                    cluster.push_back(cl);
                    SiPMadresses.erase(SiPMadresses.begin());
                    //candidates.erase(candidates.begin());
                
                for(int i=0; i<SiPMadresses.size();i++)
                {
                    cluster_size=cluster.size();
                    for(std::vector<UInt_t> j : cluster)
                    {
                        cl.clear();
                        //std::cout<<"B "<<SiPMadresses[i][0]<<" "<<i<<std::endl;
                        if (j[1]==SiPMadresses[i][1] and j[4]==SiPMadresses[i][4] and abs(j[2]-SiPMadresses[i][2])<=1 and abs(j[3]-SiPMadresses[i][3])<=1)
                        {
                            //std::cout<<"F "<<SiPMadresses[i][0]<<" "<<i<<std::endl;
                            cl.push_back(SiPMadresses[i][0]);
                            cl.push_back(SiPMadresses[i][1]);
                            cl.push_back(SiPMadresses[i][2]);
                            cl.push_back(SiPMadresses[i][3]);
                            cl.push_back(SiPMadresses[i][4]);
                            cluster.push_back(cl);
                            couter.push_back(i);
                            //candidates.erase(candidates.begin()+i);
                            //std::cout<<"L "<<candidates.size()<<std::endl;
                            break;
                        }
                    }
                    if(cluster_size==cluster.size())
                        {
                            not_cluster=not_cluster+1;
                        }
                    } 
                for(int i=0;i<SiPMadresses.size();i++){
                    a=0;
                    for(int j=0; j<couter.size(); j++){
                        //std::cout<<"bla "<<couter[j]<<" "<<i<<std::endl;
                        if(i==couter[j]){
                            a++;
                        }
                        //std::cout<<"a "<<a<<std::endl;
                    }
                    if(a==0){
                        //std::cout<<"C "<<SiPMadresses[i][0]<<" "<<i<<std::endl;
                        candidates.push_back(SiPMadresses[i]);
                    }
                }
                }
                

                
                
                clusters.push_back(cluster); 
                SiPMadresses=candidates;
            }
            
// clusters_final is vector of vector of event's idx 

        for(int i=0; i<clusters.size(); i++)
        {
            //std::cout<<"new cluster"<<std::endl;
            for(int j=0;j<clusters[i].size();j++){
            //std::cout<<clusters[i][j][2]<<" "<<clusters[i][j][3]<<" "<<i<<" "<<clusters[i][j][4]<<std::endl;
            }
        }
        std::vector<std::vector<UInt_t>> clusters_final;
        std::vector<UInt_t> cl_f;
        for(int i=0; i<clusters.size(); i++)
        {
            cl_f.clear();
            for(int j=0;j<clusters[i].size();j++){
                cl_f.push_back(clusters[i][j][0]);
            }
        clusters_final.push_back(cl_f);
        }
        
        for(int i=0; i<clusters_final.size(); i++)
        {
            //std::cout<<"new cluster"<<std::endl;
            for(int j=0;j<clusters_final[i].size();j++){
                //std::cout<<"cluster_finals "<<clusters_final[i][j]<<std::endl;
                
            }
        }
       
        pSiPMGeomPar = dynamic_cast<SSiPMsGeomPar*>(pm()->getParContainer("SiPMsGeomPar"));
        if (!pSiPMGeomPar)
        {
            std::cerr << "Parameter container 'SiPMsGeomPar' was not obtained! Check if it exists in params.txt" << std::endl;
            exit(EXIT_FAILURE);
        }
        

        
        
        
        
        
        
        
//    //change the code below by inserting the fiber identification algorithm and based on the algorithm, fill the allFibData structure for all subevents
//    int n_subevents = 5; //number of subevents
//    for (int i = 0; i < n_subevents; i++)
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
             
             
    
return allFibData;    
}

