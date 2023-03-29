#include "STP4to1Source.h"
#include "SFibersIdentification.h"
#include "SUnpacker.h"
#include "SLookup.h"
#include "SSiPMHit.h"
#include "SCategory.h"
#include "SiFi.h"


#include "SPar.h"
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
#include"SSiPMCluster.h"
#include "SMultiFibersLookup.h"
#include "SSiPMClusterFinder.h"

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
    //catSiPMsCluster = sifi()->getCategory(SCategory::CatSiPMClus);
   
    SCategory * catSiPMsCluster = sifi()->getCategory(SCategory::CatSiPMClus);
    Int_t num_clas=catSiPMsCluster->getEntries();
    //std::cout<<"Clus: "<<num_clas<<std::endl;
    
    SCategory * catSiPMHit = sifi()->getCategory(SCategory::CatSiPMHit);
    
    Int_t num_hit=catSiPMHit->getEntries();
    //std::cout<<num_hit<<std::endl;

    std::vector<std::shared_ptr<identifiedFiberData>> allFibData;
        
    std::vector<std::vector<UInt_t>> SiPMadresses;
    std::vector<UInt_t> ja;
 
    SSiPMsChannel* lc;
    SSiPMsLookupTable* pLookUp = dynamic_cast<SSiPMsLookupTable*>(pm()->getLookupContainer("TPLookupTable"));
    
    if(hits.size() > 0){
        
        int n_hits = hits.size();
        
        
  //ja vector: hit number, module, layer, element, side (0- right, 1- left)

        for(int j = 0; j <n_hits; j++)
        {

                lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[j]->channelID));

                
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

 // algorithm for clustering- ugly version          
           std::vector<std::vector<UInt_t>> cluster;
           std::vector<UInt_t> cl;
           int not_cluster=1;
           std::vector<std::vector<std::vector<UInt_t>>> clusters;
           std::vector<std::vector<UInt_t>> candidates;
           std::vector<int> couter;
           int cluster_size;
           int a=0;

            while(not_cluster){
                candidates.clear();
                couter.clear();
                not_cluster=0;
                cluster.clear();
                cl.clear();
                if(SiPMadresses.size()>1){
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
                        if (j[1]==SiPMadresses[i][1] and j[4]==SiPMadresses[i][4] and abs((int)j[2]-(int)SiPMadresses[i][2])<=1 and abs((int)j[3]-(int)SiPMadresses[i][3])<=1)
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
                else{
                    if(SiPMadresses.size()==1){
                    cl.push_back(SiPMadresses[0][0]);
                    cl.push_back(SiPMadresses[0][1]);
                    cl.push_back(SiPMadresses[0][2]);
                    cl.push_back(SiPMadresses[0][3]);
                    cl.push_back(SiPMadresses[0][4]);
                    cluster.push_back(cl);
                    SiPMadresses.erase(SiPMadresses.begin());
                    }
                }

                clusters.push_back(cluster); 
                SiPMadresses=candidates;
            }
            
// clusters_final is vector of vector of event's idx 

std::vector<std::vector<UInt_t>> clusters_final;
if(clusters.size())
{
        for(int i=0; i<clusters.size(); i++)
        {
            for(int j=0;j<clusters[i].size();j++){
            //std::cout<<clusters[i][j][2]<<" "<<clusters[i][j][3]<<" "<<i<<" "<<clusters[i][j][4]<<std::endl;
            }
        }
        std::vector<UInt_t> cl_f;

        for(int i=0; i<clusters.size(); i++)
        {
            cl_f.clear();
            for(int j=0;j<clusters[i].size();j++){
                cl_f.push_back(clusters[i][j][0]);
            }
        clusters_final.push_back(cl_f);
        }
}
        
        std::vector<std::vector<UInt_t>> scat_bottom;
        std::vector<std::vector<UInt_t>> ab_bottom;
        std::vector<std::vector<UInt_t>> scat_top;
        std::vector<std::vector<UInt_t>> ab_top;
       // }
        

if(clusters_final.size()){
        
        for(int i=0; i<clusters_final.size(); i++){
            lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[clusters_final[i][0]]->channelID ));
            if(lc->m==0 and lc->side=='l'){
                scat_bottom.push_back(clusters_final[i]);
            }
            if(lc->m==1 and lc->side=='l'){
                ab_bottom.push_back(clusters_final[i]);
            }
            if(lc->m==0 and lc->side=='r'){
                scat_top.push_back(clusters_final[i]);
            }
            if(lc->m==1 and lc->side=='r'){
                ab_top.push_back(clusters_final[i]);
            }
        }
 //------------------------------------------------------------------------------------------------------------      
    Float_t x_offset=0;
    Float_t y_offset=0;
    Float_t z_offset=0;
    
    Float_t fiber_pitch=1;
    
    Float_t x_position=0;
    Float_t y_position=0;
    Float_t z_position=0;
    
    Float_t x_COG=0;
    Float_t y_COG=0;
    Float_t z_COG=0;
    
    Float_t weights=0;
    
    Float_t x_cluster=0;
    Float_t y_cluster=0;
    Float_t z_cluster=0;
    Int_t mod = 0;
    Int_t lay = 0;
    Int_t element = 0;
    char side = '\0';
    Int_t swID=0;
    std::vector<Int_t> swIDs_bottom;
    std::vector<Int_t> swIDs_top;
        
    //if(clusters_final.size()!=0){
    
        for(int i=0; i<clusters_final.size(); i++){
            
             x_position=0;
             y_position=0;
             z_position=0;
            
             x_COG=0;
             y_COG=0;
             z_COG=0;
            
             weights=0;
            
             x_cluster=0;
             y_cluster=0;
             z_cluster=0;
            for(int j=0; j<clusters_final[i].size(); j++){

                lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[clusters_final[i][j]]->channelID ));
                  
                   x_position=(Float_t)lc->l;
                   y_position=(Float_t)lc->m; 
                   z_position=(Float_t)lc->element;
                   side=lc->side;
                   
                   x_COG=x_COG+hits[clusters_final[i][j]]->energy*x_position;
                   y_COG=y_COG+hits[clusters_final[i][j]]->energy*y_position;
                   z_COG=z_COG+hits[clusters_final[i][j]]->energy*z_position;
                   
                   weights=weights+hits[clusters_final[i][j]]->energy;
                //std::cout<<"cluster mod, lay, elemnt, x_pos, y_pos, z_pos: "<<mod<<" "<<lay<<" "<<element<<" "<<x_position<<" "<<y_position<<" "<<z_position<<" "<<weights<<" "<<x_COG<<" "<<y_COG<<" "<<z_COG<<std::endl;
                   
            }
            x_cluster=x_COG/weights;
            y_cluster=y_COG/weights;
            z_cluster=z_COG/weights;

            
        if(side=='l'){
            swID=floor(x_cluster)*28+floor(z_cluster);
            swIDs_bottom.push_back(swID);
            //std::cout<<side<<" "<<x_cluster<<" "<<y_cluster<<" "<<z_cluster<<" "<<swID<<std::endl;
        }
        if(side=='r'){
            swID=368+floor(x_cluster)*28+floor(z_cluster);
            swIDs_top.push_back(swID);
            //std::cout<<side<<" "<<x_cluster<<" "<<y_cluster<<" "<<z_cluster<<" "<<swID<<std::endl;
        }
        
        
        }
 //------------------------------------------------------------------------------------------------------------      
        
        
        SMultiFibersChannel* lc2;
        SMultiFibersLookupTable* pLookUp2;
        pLookUp2 = dynamic_cast<SMultiFibersLookupTable*>(pm()->getLookupContainer("4to1SiPMtoFibersLookupTable") );
        std::vector<std::vector<std::string> > vec ;
        std::vector<std::vector<std::vector<std::string>>> cluster_fibers_sb;
        std::vector<std::vector<std::vector<std::string>>> cluster_fibers_st;
        std::vector<std::vector<std::string>> SiPMfibers;
 
//normal
      /* if (scat_bottom.size()){
        for(int i=0; i<scat_bottom.size(); i++){
            for(int j : scat_bottom[i]){
                lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[j]->channelID));    
                lc2= dynamic_cast<SMultiFibersChannel*>(pLookUp2->getAddress(0x1000, lc->s));
                if(lc->s == 800)
                    continue;
                vec = lc2->vecFiberAssociations;

                //std::cout<<"SiPMId: "<<lc->s<<std::endl;
                //std::cout<<"vec size: "<<vec.size()<<std::endl;
                for(std::vector<std::string> k : vec){
                    SiPMfibers.push_back(k);
                }
            }
            cluster_fibers_sb.push_back(SiPMfibers);
            SiPMfibers.clear();
        }
        }
        
        for(int i=0; i<scat_top.size(); i++){
            for(int j : scat_top[i]){
                lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[j]->channelID));    
                lc2= dynamic_cast<SMultiFibersChannel*>(pLookUp2->getAddress(0x1000, lc->s));
                if(lc->s == 800)
                    continue;
                vec = lc2->vecFiberAssociations;
                for(std::vector<std::string> k:vec){
                    SiPMfibers.push_back(k);
                }
            }
            cluster_fibers_st.push_back(SiPMfibers);
            SiPMfibers.clear();
        }*/
    
//-------------------------------------------------------------------------------------
       if (swIDs_bottom.size()){
        for(int i=0; i<swIDs_bottom.size(); i++){
                //lc = dynamic_cast<SSiPMsChannel*>(pLookUp->getAddress(0x1000,hits[j]->channelID));    
                lc2= dynamic_cast<SMultiFibersChannel*>(pLookUp2->getAddress(0x1000, swIDs_bottom[i]));
                //if(lc->s == 800)
                    //continue;
                //std::cout<<swIDs_bottom[i]<<std::endl;
                if(swIDs_bottom[i]<0 or swIDs_bottom[i]>112)
                    continue;
                vec = lc2->vecFiberAssociations;
                //std::cout<<swIDs_bottom[i]<<std::endl;
                //std::cout<<"SiPMId: "<<lc->s<<std::endl;
                //std::cout<<"vec size: "<<vec.size()<<std::endl;
                for(std::vector<std::string> k : vec){
                    SiPMfibers.push_back(k);
                }
            //}
            cluster_fibers_sb.push_back(SiPMfibers);
            SiPMfibers.clear();
        }
        }
        if (swIDs_top.size()){
        for(int i=0; i<swIDs_top.size(); i++){
            //for(int j : scat_top[i]){
                
                lc2= dynamic_cast<SMultiFibersChannel*>(pLookUp2->getAddress(0x1000, swIDs_top[i]));
                //std::cout<<swIDs_top[i]<<std::endl;
                //std::cout<<swIDs_top[i]<<std::endl;
                if(swIDs_top[i]<368 or swIDs_top[i]>479)
                    continue;
                vec = lc2->vecFiberAssociations;
                for(std::vector<std::string> k:vec){
                    SiPMfibers.push_back(k);
                }
            //}
            cluster_fibers_st.push_back(SiPMfibers);
            SiPMfibers.clear();
        }
        }
//------------------------------------------------------------
        


        std::vector<std::vector<std::string>> in_fiber;
        std::vector<std::vector<std::vector<std::string>>> cluster_in_fibers;
        if(cluster_fibers_sb.size() and cluster_fibers_st.size()){
        for(int i=0;i<cluster_fibers_sb.size();i++)
        {
            for(int j=0; j<cluster_fibers_st.size();j++)
            {
                for(std::vector<std::string> fibers_bottom : cluster_fibers_sb[i])
                {
                    for(std::vector<std::string> fibers_top : cluster_fibers_st[j])
                    {
                        if(fibers_bottom[0]==fibers_top[0] and fibers_bottom[1]==fibers_top[1] and fibers_bottom[2]==fibers_top[2])
                        {
                            fibers_bottom.push_back(std::to_string(i));
                            fibers_bottom.push_back(std::to_string(j));
                            in_fiber.push_back(fibers_bottom);
                            //std::cout<<fibers_bottom[0]<<" "<<fibers_bottom[1]<<" "<<fibers_bottom[2]<<" "<<fibers_bottom[3]<<" "<<fibers_bottom[4]<<" "<<fibers_bottom[5]<<std::endl;
                        }
                    }
                }
                cluster_in_fibers.push_back(in_fiber);
                in_fiber.clear();
            }
        }
        }

        
        int qdc_l=0;
        int time_l=0;
        int qdc_r=0;
        int time_r=0;
        
        for(std::vector<std::vector<std::string>> k :cluster_in_fibers)
        {
            if(k.size()==1){
               for(int i : scat_bottom[std::stoi(k[0][4])]){
 
                qdc_l=qdc_l+hits[i]->energy;
                time_l=hits[i]->time;
               }
               for(int i: scat_top[std::stoi(k[0][5])]){
 
                qdc_r=qdc_l+hits[i]->energy;
                time_r=hits[i]->time;
               }
                fibData->energyL=qdc_l;
                fibData->timeL=time_l;
                fibData->energyR=qdc_r;
                fibData->timeR=time_r;
                fibData->mod=std::stoi(k[0][0]);
                fibData->lay=std::stoi(k[0][1]);
                fibData->fi=std::stoi(k[0][2]);
                allFibData.push_back(fibData);
            }
        }

             SiPMadresses.clear();
             candidates.clear();
             clusters.clear();
             clusters_final.clear();
}
else{
               fibData->energyL=-100.0;
                fibData->timeL=-100.0;
                fibData->energyR=-100.0;
                fibData->timeR=-100.0;
                fibData->mod=-100;
                fibData->lay=-100;
                fibData->fi=-100;
                allFibData.push_back(fibData);
                
}
    }
else{
                fibData->energyL=-100.0;
                fibData->timeL=-100.0;
                fibData->energyR=-100.0;
                fibData->timeR=-100.0;
                fibData->mod=-100;
                fibData->lay=-100;
                fibData->fi=-100;
                allFibData.push_back(fibData);
}
    
    
             
return allFibData;    
}
