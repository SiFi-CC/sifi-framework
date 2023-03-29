// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SSiPMClusterFinder.h"
#include "SSiPMCluster.h"
#include "SCategory.h"
#include "SSiPMHit.h"
#include "SLocator.h"
#include "SPar.h"
#include "SiFi.h"

#include <RtypesCore.h>
#include <TObject.h>
#include <TVector3.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory> 
#include <utility>
#include <vector>

/**
 * \class SSiPMClusterFinder
 * \ingroup lib_fibers
 *
 * A task class, which finds clusters of SiPM hits. A cluster consists of SiPMs which are direct
 * neighbours (touching sides or corners).
 * 
 **/

bool SSiPMClusterFinder::init()
{
    catSiPMsHit = sifi()->buildCategory(SCategory::CatSiPMHit);
    
    if(!catSiPMsHit)
    {
        std::cerr << "No CatSiPMHit category!" << std::endl;
        return false;
    }
    
   catSiPMsCluster = sifi()->buildCategory(SCategory::CatSiPMClus);
   
   if(!catSiPMsCluster)
   {
       std::cerr << "No CatSiPMClus category!" << std::endl;
   }
    
    return true;
}

bool checkIfNeighbours(SSiPMHit* hit_1, SSiPMHit* hit_2)
{
 
    /// TODO checking if two given sipms are neighbours
    return true;    
}


bool SSiPMClusterFinder::execute()
{
/////
        std::vector<std::vector<UInt_t>> SiPMadresses;
        std::vector<UInt_t> ja;
/////
    
    int size = catSiPMsHit->getEntries();
    for (int i = 0; i < size; ++i)
    {
        SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(i));
        if (!pHit)
        {
            printf("SiPMHit doesn't exists!\n");
            continue;
        }
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t element = 0;
        char side = '\0';
        pHit->getAddress(mod, lay, element, side);

/////
        ja.push_back(i);
        ja.push_back(mod);
        ja.push_back(lay);
        ja.push_back(element);


        if(side=='r'){
            ja.push_back(0);

        }
        if(side=='l'){
            ja.push_back(1);

        }
        SiPMadresses.push_back(ja);
        ja.clear();
/////
    }    
/////

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
            

        
       
        
   //Wziąć z params
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
    Int_t aa=0;
        
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
            SLocator loc(1);
            loc[0] = i;
            SSiPMCluster* pClus = dynamic_cast<SSiPMCluster*>(catSiPMsCluster->getObject(loc));
            if (!pClus)
            {
                pClus = reinterpret_cast<SSiPMCluster*>(catSiPMsCluster->getSlot(loc));
                pClus = new (pClus) SSiPMCluster;
                pClus->Clear();
            }
            pClus->setAddress(i);
            for(int j=0; j<clusters_final[i].size(); j++){
                     mod = 0;
                     lay = 0;
                     element = 0;
                     side = '\0';
                    SSiPMHit* pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(clusters_final[i][j]));
                    if (!pHit)
                   {
                     printf("SiPMHit doesn't exists!\n");
                     continue;
                   }
                   pHit->getAddress(mod, lay, element, side);
                  
                   x_position=(Float_t)lay;
                   y_position=(Float_t)mod; //DOPYTAĆ!!!!
                   z_position=(Float_t)element;
                   
                   x_COG=x_COG+pHit->getQDC()*x_position;
                   y_COG=y_COG+pHit->getQDC()*y_position;
                   z_COG=z_COG+pHit->getQDC()*z_position;
                   
                   weights=weights+pHit->getQDC();
                //std::cout<<"cluster mod, lay, elemnt, x_pos, y_pos, z_pos: "<<mod<<" "<<lay<<" "<<element<<" "<<x_position<<" "<<y_position<<" "<<z_position<<std::endl;
                pClus->addHit(clusters_final[i][j]);
                pClus->catSiPMsHit = catSiPMsHit;
                
                if(j==0){
                    aa=0;
                }
                else{
                    aa=1;
                }
                   
            }
            x_cluster=x_COG/weights;
            y_cluster=y_COG/weights;
            z_cluster=z_COG/weights;
            if(aa==0){
                x_cluster=-1;
                y_cluster=-1;
                z_cluster=-1;
            }
            if(side=='r'){
                x_cluster=-1;
                y_cluster=-1;
                z_cluster=-1;
            }
                
            pClus->getPoint().SetXYZ(x_cluster, y_cluster, z_cluster);
            //pClus->print();
            //std::cout<<"cluster X,Y,Z: "<<x_cluster<<" "<<y_cluster<<" "<<z_cluster<<" "<<std::endl;
        }
    //}
    
             SiPMadresses.clear();
/////
             /*
        for(Int_t i=0; i < clusters_final.size(); ++i) {
            SLocator loc(1);
            loc[0] = i;
            SSiPMCluster* pClus = dynamic_cast<SSiPMCluster*>(catSiPMsCluster->getObject(loc));
            if (!pClus)
            {
                pClus = reinterpret_cast<SSiPMCluster*>(catSiPMsCluster->getSlot(loc));
                pClus = new (pClus) SSiPMCluster;
                pClus->Clear();
            }
            pClus->setAddress(i);
            for(Int_t j=0; j < clusters_final[i].size(); ++j) {
                pClus->addHit(clusters_final[i][j]);
                pClus->getPoint().SetXYZ(0, 0, 0);
                pClus->catSiPMsHit = catSiPMsHit;
            }
            pClus->print();
        }
        */

    return true;
}

bool SSiPMClusterFinder::finalize()
{
    return true;
}
