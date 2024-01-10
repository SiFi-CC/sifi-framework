#include "SFibersRawClusterFinder.h"
#include "SCategory.h"
#include "SLocator.h"
#include "SLookup.h"
#include "SSiPMHit.h"
#include "STP4to1Source.h"
#include "SUnpacker.h"
#include "STask.h"
#include "SiFi.h"
#include "SFibersRaw.h"
#include "SFibersRawCluster.h"
#include "SFibersLookup.h"
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <algorithm>          // for max, min
#include <iterator>
#include <cmath>              // for fabs
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for abort
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <map>
#include <sstream>
#include <utility> // for pair
#include <vector>  // for vector
#include <numeric>

/**
 * Constructor. Requires ...
 *
 * \param subevent subevent id
 */

SFibersRawClusterFinder::SFibersRawClusterFinder() : STask(), catSiPMsHit(nullptr), catSiPMsCluster(nullptr), catFibersRaw(nullptr), catFibersRawClus(nullptr), fibLookup(nullptr)
{}

bool SFibersRawClusterFinder::init()
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
    
    catFibersRawClus = sifi()->buildCategory(SCategory::CatFibersRawClus);
    
    if(!catFibersRawClus)
    {
        std::cerr << "No CatFibersRawClus category!" << std::endl;
        return false;
    }
    
    fibLookup = dynamic_cast<SMultiFibersLookupTable*>(pm()->getLookupContainer("4to1SiPMtoFibersLookupTable"));
    
    // get TP calibrator parameters
    pTOFPETCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FibersTOFPETCalibratorPar"));

    if (!pTOFPETCalPar)
    {
        std::cerr << "Parameter container 'STPCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    return true;
}

bool checkIfNeighboursC(identifiedFiberDataC fiber_1, identifiedFiberDataC fiber_2)
{
    int mod_1, lay_1, fib_1;
    int mod_2, lay_2, fib_2;

    mod_1 = fiber_1.mod;
    lay_1 = fiber_1.lay;
    fib_1 = fiber_1.fi;
    
    mod_2 =  fiber_2.mod;
    lay_2 =  fiber_2.lay;
    fib_2 =  fiber_2.fi;
//     std::cout << lay_1 << " " << lay_2 << " " << fib_1 << " " << fib_2 << std::endl;
    
    if(mod_1 != mod_2){ // if two hits are in different modules they are not neighbours
        std::cout << "modules not equal!" << std::endl;
        return false;
    }
    
    if(abs(lay_1 - lay_2) < 2 &&    // if two fibers have neighbouring fiber and layer numbers they are neighbours
       abs(fib_1 - fib_2) < 2)
    {
        return true;
    }
    else
        return false;
}

std::vector<int> createFiberClusters(std::vector<std::vector<identifiedFiberDataC>> fibsInClusterPairs)
   {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
        std::vector<int> nFibClustersInPairs; //returned from the function, length: n_pairs
    std::vector<std::vector<std::vector<identifiedFiberDataC>>> fibClustersInPairs;
        for (int i = 0; i < fibsInClusterPairs.size(); i++) //iterate over all cluster pairs
        {
            std::vector<identifiedFiberDataC> unassignedFibers;
            std::vector<std::vector<identifiedFiberDataC>> fibClusters;
            std::vector<identifiedFiberDataC>::iterator it;
            unassignedFibers = fibsInClusterPairs[i];
            std::vector<bool> isUnassigned;
            for(int j=0; j<unassignedFibers.size(); j++){
                isUnassigned.push_back(1);
            }
            int allFib_size = unassignedFibers.size();
            int clusterCounter = 0;
            int fibInClusterCounter = 0;
            int prev_fibInClusterCounter = -1;
            fibClusters.push_back({});
            fibClusters[clusterCounter].push_back(unassignedFibers.front());
//             std::cout << "unassignedF: " << allFib_size << std::endl; 

//             it = unassignedFibers.begin();
//             unassignedFibers.erase(it);
            fibInClusterCounter = 1;
            isUnassigned[0] = 0;
//             std::cout << "allFib_size: " << allFib_size << std::endl; 
//             std::cout << "accumulate: " << std::accumulate(isUnassigned.begin(), isUnassigned.end(), 0) << std::endl;
                while(std::accumulate(isUnassigned.begin(), isUnassigned.end(), 0)){
                    while(prev_fibInClusterCounter != fibInClusterCounter){
                        prev_fibInClusterCounter = fibClusters[clusterCounter].size();
                        for(int m=0; m<unassignedFibers.size(); m++){
                            for(int n=0; n<fibClusters[clusterCounter].size(); n++){
                                if(isUnassigned[m]){
//                                     std::cout << "CHECKIF NEIGH" << checkIfNeighboursC(unassignedFibers[m], fibClusters[clusterCounter][n]) << std::endl;
                                    if(checkIfNeighboursC(unassignedFibers[m], fibClusters[clusterCounter][n])){
                                        
                                        fibClusters[clusterCounter].push_back(unassignedFibers[m]);
                                        fibInClusterCounter=fibClusters[clusterCounter].size();
                                        isUnassigned[m] = 0;
                                        
                                    }
                                }
                            }
                        
                            
                        }
                    }
                    if(std::accumulate(isUnassigned.begin(), isUnassigned.end(), 0) != 0){
                        for(int k =0; k< isUnassigned.size(); k++){
                            if(isUnassigned[k]){
                                fibClusters.push_back({unassignedFibers[k]});
                                isUnassigned[k] = 0;
                                clusterCounter++;
                                break;
                            }
                        }              
                    }
                }

        if(clusterCounter > 0) {
            std::cout << i << "**************************8***************************clusterCounter: " << clusterCounter << std::endl;
            for(int l=0;l<fibClusters.size(); l++){
                    for(int p=0;p<fibClusters[l].size(); p++){
                    std::cout << fibClusters[l][p].mod << " " << fibClusters[l][p].lay << " " << fibClusters[l][p].fi<< std::endl;
                    
                    }
                    std::cout << "=========================="<<  std::endl;
                }
        }
        nFibClustersInPairs.push_back(clusterCounter+1);    
        fibClustersInPairs.push_back(fibClusters);
        }
    return nFibClustersInPairs;
    }

std::vector<float> getAveragePosition(std::vector<identifiedFiberDataC> fibersInPairOfClusters){
    std::vector<float> clusterCenterAddress;
    Float_t qdc_l, qdc_r, qavg, qdc_sum;
    qdc_l=0;
    qdc_r=0;
    qavg=0;
    qdc_sum=0;
    Float_t fmod, flay, ffi;
    fmod=0;
    flay=0;
    ffi=0;
    Int_t n_fibers;
    n_fibers = fibersInPairOfClusters.size();
    for(int i=0; i<n_fibers; i++){
        qavg = sqrt(fibersInPairOfClusters[i].energyL*fibersInPairOfClusters[i].energyR);
        qdc_sum += qavg;
        if(fibersInPairOfClusters[i].mod == 0){
            flay+=fibersInPairOfClusters[i].lay*qavg;
            ffi+=fibersInPairOfClusters[i].fi*qavg;
        }
        else std::cerr << "In SFibersRawClusterFinder module != 0. Check all involved fiber addresses. If using absorber, this function needs to be edited" << std::endl;
    }
    flay = flay/qdc_sum;
    ffi = ffi/qdc_sum;
    clusterCenterAddress.push_back(fmod);
    clusterCenterAddress.push_back(flay);
    clusterCenterAddress.push_back(ffi);
    return clusterCenterAddress;
}

std::vector<float> getFirstFiber(std::vector<identifiedFiberDataC> fibersInPairOfClusters){
    std::vector<float> clusterCenterAddress;
    Float_t qdc_l, qdc_r, qavg, qdc_sum;
    qdc_l=0;
    qdc_r=0;
    qavg=0;
    qdc_sum=0;
    Float_t fmod, flay, ffi;
    fmod=0;
    flay=0;
    ffi=0;
    Int_t n_fibers;
    n_fibers = fibersInPairOfClusters.size();
     int min_layer, layer;
    min_layer  = fibersInPairOfClusters[0].lay;
    for(int i=0; i<n_fibers; i++){
        layer = fibersInPairOfClusters[i].lay;
        if(layer < min_layer){
            min_layer = layer;
        }
        qavg = sqrt(fibersInPairOfClusters[i].energyL*fibersInPairOfClusters[i].energyR);
        qdc_sum += qavg;
        if(fibersInPairOfClusters[i].mod == 0){
            ffi+=fibersInPairOfClusters[i].fi*qavg;
        }
        else std::cerr << "In SFibersRawClusterFinder module != 0. Check all involved fiber addresses. If using absorber, this function needs to be edited" << std::endl;
    }
    flay = min_layer;
    ffi = ffi/qdc_sum;
    clusterCenterAddress.push_back(fmod);
    clusterCenterAddress.push_back(flay);
    clusterCenterAddress.push_back(ffi);
    return clusterCenterAddress;
}

std::vector<float> getHeaviestFiber(std::vector<identifiedFiberDataC> fibersInPairOfClusters){
    std::vector<float> clusterCenterAddress;
    Float_t qavg, qdc_sum;
    qavg=0;
    qdc_sum=0;
    Float_t fmod, flay, ffi;
    fmod=0;
    flay=0;
    ffi=0;
    Int_t n_fibers;
    n_fibers = fibersInPairOfClusters.size();
    int max_index, max_qavg;
    max_qavg  = sqrt(fibersInPairOfClusters[0].energyL*fibersInPairOfClusters[0].energyR);
    max_index = 0;
    for(int i=1; i<n_fibers; i++){
        if(fibersInPairOfClusters[i].mod == 0){
        qavg = sqrt(fibersInPairOfClusters[i].energyL*fibersInPairOfClusters[i].energyR);
        }
        else std::cerr << "In SFibersRawClusterFinder module != 0. Check all involved fiber addresses. If using absorber, this function needs to be edited" << std::endl;
        if(qavg > max_qavg){
            max_qavg = qavg;
            max_index = i;
        }
        std::cout << "largest qavg: "<<max_qavg << " for: " <<  max_index << std::endl;
    }
    flay = fibersInPairOfClusters[max_index].lay;
    ffi = fibersInPairOfClusters[max_index].fi;

    clusterCenterAddress.push_back(fmod);
    clusterCenterAddress.push_back(flay);
    clusterCenterAddress.push_back(ffi);
    return clusterCenterAddress;
}
    
identifiedFiberDataC getClusterPosition(std::vector<identifiedFiberDataC> fibersInPairOfClusters){
    Float_t qdc_l, qdc_r;
    qdc_l=0;
    qdc_r=0;
    Float_t fmod, flay, ffi;
    
    for(int i=0; i<fibersInPairOfClusters.size(); i++){
        qdc_l+=fibersInPairOfClusters[i].energyL; //qdc_l (_r) defined here as sum of all fiber's qdc's (no alignment yet!)
        qdc_r+=fibersInPairOfClusters[i].energyR;
    }

    std::vector<float> clusterCenterAddress;
    clusterCenterAddress = getHeaviestFiber(fibersInPairOfClusters); //or FF, or AP
    fmod = clusterCenterAddress[0];
    flay = clusterCenterAddress[1];
    ffi = clusterCenterAddress[2];
    std::cout << "modlayfi: " << fmod << " " << flay << " " << ffi << std::endl;
    identifiedFiberDataC clusterPosition; //qdc_l, qdc_r, address
    clusterPosition.energyL = qdc_l;
    clusterPosition.energyR = qdc_r;
    clusterPosition.mod = round(fmod);
    clusterPosition.lay = round(flay);
    clusterPosition.fi = round(ffi);
    std::cout << "avPOs: " <<clusterPosition.energyL << " " << clusterPosition.energyR << " " << clusterPosition.mod << " " << clusterPosition.lay << " " << clusterPosition.fi << std::endl;
    return clusterPosition;
}

bool SFibersRawClusterFinder::execute()
{ 
    const int N_MODULES = 1;
    
    SLocator loc(3);
    
    std::vector<SSiPMCluster*> ClustersTop[N_MODULES];
    std::vector<SSiPMCluster*> ClustersBottom[N_MODULES];
    std::vector<SSiPMHit*> SiPMsTop[N_MODULES];
    std::vector<SSiPMHit*> SiPMsBottom[N_MODULES];
//     std::vector<fibAddressC> FiberAddresses;
    std::vector<SFibersRaw*> fibersRaw;
    std::vector<SFibersRawCluster*> fibersRawCluster;
    
    size_t nClus = catSiPMsCluster->getEntries();
    if(nClus < 2) return true;
    
    Int_t mod;
    char side;
    SSiPMCluster* pCluster;
    
    for (uint j = 0; j < nClus; ++j) // take clusters from the SSiPMCluster category, sort them into two groups: top and bottom
    {
        pCluster = (SSiPMCluster *)catSiPMsCluster->getObject(j);
        pCluster->getAddress(mod, side);

        if(side=='r')
            ClustersTop[mod].push_back(pCluster);
        else 
            ClustersBottom[mod].push_back(pCluster);
    }
    
    size_t nSiPMHit = catSiPMsHit->getEntries();
    
    Int_t s_mod, s_lay, s_el;
    char s_side;
    SSiPMHit* pHit;
    
    for (uint j = 0; j < nSiPMHit; ++j) // AUXILIARY: take SiPMs from the SSiPMHit category, sort them into two groups: top and bottom
    {
        pHit = (SSiPMHit *)catSiPMsHit->getObject(j);
        pHit->getAddress(s_mod, s_lay, s_el, s_side);
        
        if(s_side=='r')
            SiPMsTop[s_mod].push_back(pHit);
        else 
            SiPMsBottom[s_mod].push_back(pHit);
    }
    
    
    
    for(int m = 0; m < N_MODULES; ++m)
    {
        if(ClustersBottom[m].size() == 0 || ClustersTop[m].size() == 0) return true;
        
        std::unique_ptr<int[]> FiberBottom(new int[ClustersBottom[mod].size()]);
        std::unique_ptr<int[]> FiberTop(new int[ClustersTop[mod].size()]);
        
        for(int k=0; k<ClustersBottom[mod].size(); ++k)
        {
            FiberBottom[k]=0;
        }
        
        for(int l=0; l<ClustersTop[mod].size(); ++l)
        {
            FiberTop[l]=0;
        }
        
        for(int k=0; k<ClustersBottom[mod].size(); ++k)
        {

            for(int l=0; l<ClustersTop[mod].size(); ++l)
            {
                std::vector<identifiedFiberDataC> fib = CommonFibers(ClustersTop[mod][l],ClustersBottom[mod][k]);
//                 FiberAddresses.insert(FiberAddresses.end(),fib.begin(),fib.end());
//                 for(int f=0; f<fib.size(); ++f)
//                 {
//                     std::cout << "fibContents: " << fib[f].mod << " " << fib[f].lay << " " << fib[f].fi << std::endl;                }
                for(int f=0; f<fib.size(); ++f)
                {
                    SFibersRaw *fr = new SFibersRaw();
                    fr->setAddress(fib[f].mod, fib[f].lay, fib[f].fi);
                    fr->setQDCL(alignQDC(fib[f], ClustersBottom[mod][k]->getQDC()));
                    fr->setQDCR(alignQDC(fib[f], ClustersTop[mod][l]->getQDC()));
                    fr->setTimeL(ClustersBottom[mod][k]->getTime());
                    fr->setTimeR(ClustersTop[mod][l]->getTime());
                    Int_t id_t, id_b;
                    ClustersTop[mod][l]->getID(id_t);
                    ClustersBottom[mod][k]->getID(id_b);
                    fr->setClusters(id_t, id_b);
                    
                    fibersRaw.push_back(fr);
                }
                
                fib.clear();
            }
        }
    }
    //TODO add selection of valid events below!
    
    std::vector<int> topClusters; 
    std::vector<int> botClusters;
    std::vector<std::vector<int>> clusterPairs; //vectors of length 3: topClusterID, botClusterID, multiplicity
    /*std::vector<std::vector<std::vector<int>>> fibersInClusterPairs;*/ //for each clusterPair (in clusterPairs), store a vector of adressess=3-vectors=m,l,f of all involved fibers; TODO change the innermost vector to fibAdress.
    std::vector<std::vector<identifiedFiberDataC>> fibersInClusterPairs;
    
    int topClusID, botClusID;
    int prevTopClusID, prevBotClusID;
    topClusID=-1;
    botClusID=-1;
    prevTopClusID=-1;
    prevBotClusID=-1;
    int clusterFiberMult = 1;

// create vector of cluster pairs - works properly.
    if(!fibersRaw.empty()) {
        fibersRaw[0]->getClusters(topClusID, botClusID);
        prevTopClusID = topClusID;
        prevBotClusID = botClusID;
        clusterFiberMult = 1;
        for(int f=0; f<fibersRaw.size(); f++){
            int mod, lay, fi;
            char side;
            prevTopClusID = topClusID;
            prevBotClusID = botClusID;
            fibersRaw[f]->getClusters(topClusID, botClusID);
            fibersRaw[f]->getAddress(mod,lay,fi);
//             std::cout << " topClusID | botClusID: " <<  topClusID << " " << botClusID << std::endl;
//             std::cout << " topClusID | botClusID: " <<  topClusID << " " << botClusID << " fibAdress: " << mod << " " << lay << " " << fi << std::endl;

            
            if (prevTopClusID != topClusID || prevBotClusID != botClusID){
                clusterPairs.push_back({prevTopClusID, prevBotClusID, clusterFiberMult-1});
                clusterFiberMult = 1;
                prevTopClusID = topClusID;
                prevBotClusID = botClusID;
            }
            if(f == fibersRaw.size()-1){
                clusterPairs.push_back({topClusID, botClusID, clusterFiberMult});
            }
            clusterFiberMult++;
        }
        for(int i=0;i<clusterPairs.size(); i++){
            int mod, lay, fi;
            char side;
            float energyl, energyr;
            Long64_t timel,timer;
            int topClusID, botClusID;
            fibersInClusterPairs.push_back({});
            std::cout << i << "-th cluster pair: " <<clusterPairs[i][0] << " " << clusterPairs[i][1] << " f mult: " << clusterPairs[i][2] <<  std::endl;
            for(int j=0; j<fibersRaw.size(); j++){
                fibersRaw[j]->getClusters(topClusID, botClusID);
                if(topClusID == clusterPairs[i][0] && botClusID == clusterPairs[i][1]){
                    fibersRaw[j]->getAddress(mod,lay,fi);
                    energyl=fibersRaw[j]->getQDCL();
                    energyr=fibersRaw[j]->getQDCR();
                    timel=fibersRaw[j]->getTimeL();
                    timer=fibersRaw[j]->getTimeR();
                    identifiedFiberDataC f;
                    f.mod = mod;
                    f.lay = lay;
                    f.fi = fi;
                    f.side = side;
                    f.timeL = timel;
                    f.timeR = timer;
                    f.energyL = energyl;
                    f.energyR = energyr;
                    fibersInClusterPairs[i].push_back(f);
                    std::cout << "fiber: " <<  f.mod << " " << f.lay << " " << f.fi << " QDCL: " << energyl << " QDCL: " << energyr <<   std::endl;
                }
            }
        }
    }


//  labeling event classes - correct labeling, but creation of fiber clusters to be checked
    fiberClusterHitTypeC fibClusType; 
    int topIter = 0;
    int botIter = 0;
    std::vector<int> foundFiberClusters;
    std::vector<std::vector<std::vector<identifiedFiberDataC>>> foundFiberClustersInPairs;
    if(clusterPairs.size()==1){
        if(clusterPairs[0][2]==1){ // check if multiplicity of the cluster = 1
            fibClusType=uniqueClusterUniqueFiberC; // in this case, simply fill the SFibersRawCluster with SFibersRaw contents (no clusters created, only single-fiber-events)
            std::cout << "label: unique, single fiber" << std::endl;
            SFibersRawCluster *frc = new SFibersRawCluster();
            
            frc->setAddress(fibersInClusterPairs[0][0].mod, fibersInClusterPairs[0][0].lay, fibersInClusterPairs[0][0].fi);
            
            fibersInClusterPairs[0][0].side = 'l';
            frc->setQDCL(alignQDC(fibersInClusterPairs[0][0], fibersInClusterPairs[0][0].energyL));
            frc->setTimeL(fibersInClusterPairs[0][0].timeL);
            fibersInClusterPairs[0][0].side = 'r';
            frc->setQDCR(alignQDC(fibersInClusterPairs[0][0], fibersInClusterPairs[0][0].energyR));
            frc->setTimeR(fibersInClusterPairs[0][0].timeR);
            frc->setFiberClusterLabel(fibClusType);
            fibersRawCluster.push_back(frc);
        }
        else{
            std::cout << "label: unique, mult fibers, clusterPairs.size()==1 "  << std::endl;
            fibClusType=uniqueClusterMultipleFiberC; //in this case, calculate cluster position and qdc with one of the methods: FF, HF, AP (source: Jonas' PhD thesis)
            //below, set correct values to the frc.
            
            //here add checking if neighbours
            foundFiberClusters = createFiberClusters(fibersInClusterPairs); //if only one fiber cluster, proceed. else if 2 fiber clusters -> ?. else ->skip.
            //             foundFiberClustersInPairs = createFiberClusters(fibersInClusterPairs);
            int mod, lay, fi;
            char side;
            float energyl, energyr;
            Long64_t timel,timer; // time the same within one cluster pair, regardless of side (time is rewritten from SiPM Cluster, which defines time as time of the first hit) - to be simplified?
            identifiedFiberDataC fibClusterData;
            std::cout << "MULTIPLICITY OF CLUSTER: " << clusterPairs[0][2] << std::endl;
            for(int k=0; k< fibersInClusterPairs[0].size(); k++){
                std::cout << "fibersInClusterPairs content: " << fibersInClusterPairs[0][k].mod << " "<< fibersInClusterPairs[0][k].lay << " "<< fibersInClusterPairs[0][k].fi << " "  << fibersInClusterPairs[0][k].energyL << " "<< fibersInClusterPairs[0][k].energyR << " "<< fibersInClusterPairs[0][k].timeL << " " << fibersInClusterPairs[0][k].timeR  << " "<< std::endl;
            }
            SFibersRawCluster *frc = new SFibersRawCluster();
            fibClusterData = getClusterPosition(fibersInClusterPairs[0]);
            frc->setAddress(fibClusterData.mod, fibClusterData.lay, fibClusterData.fi);
            frc->setQDCL(fibClusterData.energyL);
            frc->setQDCR(fibClusterData.energyR);
            frc->setTimeL(fibersInClusterPairs[0][0].timeL);
            frc->setTimeR(fibersInClusterPairs[0][0].timeR);
            frc->setFiberClusterLabel(fibClusType);
            frc->setFiberMultInCluster(clusterPairs[0][2]);
            frc->setNFibersClusters(foundFiberClusters[0]);
            fibersRawCluster.push_back(frc);

//             std::vector<Int_t> clustersBottomSiPMArray;
//             std::vector<Int_t> clustersTopSiPMArray;
//             Int_t smod, slay, sel;
//             char sside;
//             
//                 for(int l=0;l<ClustersBottom[mod].size(); l++){
// //                     std::cout << "Bottom" << std::endl;
//                     clustersBottomSiPMArray = ClustersBottom[mod][l]->getHitsArray();
//                     for(int m=0; m<clustersBottomSiPMArray.size(); m++){
// //                         SiPMsBottom[mod][m]->print();
//                     }
// //                     ClustersBottom[mod][l]->print();
//                 }
//                 for(int l=0;l<ClustersTop[mod].size(); l++){
// //                     std::cout << "Top" << std::endl;
//                     clustersTopSiPMArray = ClustersTop[mod][l]->getHitsArray();
//                     for(int m=0; m<clustersTopSiPMArray.size(); m++){
// //                          SiPMsTop[mod][m]->print();
//                     }
// //                     ClustersTop[mod][l]->print();
//                 }

//             fibersRawCluster[0]->setNFibersClusters(foundFiberClusters[0]);
        }
    

    
    }
    else if (clusterPairs.size()>1){
        for(int i=0;i<clusterPairs.size(); i++){
            for(int j=0;j<clusterPairs.size(); j++){
                if(clusterPairs[i][0] == clusterPairs[j][0]) topIter++;
                if(clusterPairs[i][1] == clusterPairs[j][1]) botIter++;
            }
            if(topIter == 1 && botIter == 1) {
                std::cout << "label: unique, mult fibers, clusterPairs.size()>1 " << std::endl;
                fibClusType=uniqueClusterMultipleFiberC;
//                 SFibersRawCluster *frc = new SFibersRawCluster();
//                 frc->setAddress(0,3,4);
//                 frc->setQDCL(665.7);
//                 frc->setQDCR(758.8);
//                 frc->setTimeL(1309865);
//                 frc->setTimeR(509847);
//                 frc->setFiberClusterLabel(fibClusType);
//                 fibersRawCluster.push_back(frc);
//                 fibersRawCluster[i]->setFiberMultInCluster(clusterPairs[i][2]);
                //here also add checking if neighbours
//                 foundFiberClustersInPairs = createFiberClusters(fibersInClusterPairs);
                foundFiberClusters = createFiberClusters(fibersInClusterPairs);
                    for(int l=0;l<ClustersBottom[mod].size(); l++){
//                         std::cout << "ClustersBottom"<<  ClustersBottom[mod][l]->print() <<  std::endl;
//                         ClustersBottom[mod][l]->print();
                    }
                    for(int l=0;l<ClustersTop[mod].size(); l++){
//                         ClustersTop[mod][l]->print();
//                         std::cout << "ClustersTop"<<  ClustersTop[mod][l]->print() <<  std::endl;
                    }
//                 fibersRawCluster[i]->setNFibersClusters(foundFiberClusters[i]);
            }
            else if(topIter == 2 && botIter == 1)  {
                std::cout << "label: semi-unique BOT cluster pair(topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=bottomUniqueClusterC;
            }
            else if(topIter == 1 && botIter == 2)  {
                std::cout << "label: semi-unique TOP cluster pair(topITER, botITER) = "     << topIter << " " << botIter << std::endl;
                fibClusType=topUniqueClusterC;
            }
            else {
                std::cout << "label: ambiguous cluster pair: (topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=ambiguousClusterC;
            }
//             fibersRawCluster[i]->setFiberClusterLabel(fibClusType); 
            topIter = 0;
            botIter = 0;
        }
    }
    else std::cerr << "clusterPairs.size() <= 0. Incorrect." << std::endl;
    
    
    for(int f=0; f<fibersRawCluster.size(); ++f) //size of fibersRawCluster has to be correct for this to work properly (it is currently the size of fibersRaw)
    {
//         if(fibersRaw.size() != 1)   // only events in which single fiber is uniquely identified are valid TEMPORARY
//             continue;
        

        int mod, lay, fib;
        fibersRawCluster[f]->getAddress(mod, lay, fib);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;
        
        std::cout << fibersRawCluster[f]->getQDCL() << std::endl;
        std::cout << fibersRawCluster[f]->getQDCR() << std::endl;
        std::cout << mod << " " << lay << " " << fib << std::endl;
//         SFibersRaw *pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
//         pRaw = new (pRaw) SFibersRaw(*fibersRaw[f]);
        SFibersRawCluster *pRaw = reinterpret_cast<SFibersRawCluster*>(catFibersRawClus->getSlot(loc));
        pRaw = new (pRaw) SFibersRawCluster(*fibersRawCluster[f]);
    }

    return true;
}



float SFibersRawClusterFinder::alignQDC(identifiedFiberDataC address, float qdc)
{
    SFibersChannel chan;
    std::cout << address.mod << std::endl;
    chan.m = address.mod;
    std::cout << address.lay << std::endl;
    chan.l = address.lay;
    std::cout << address.fi << std::endl;
    chan.s = address.fi;
    std::cout << address.side << std::endl;
    chan.side = address.side;
    
    auto _cpar = pTOFPETCalPar->getPar(&chan);
    auto&& cpar = *_cpar;
    
    if(qdc==-100)
    {
        std::cout<<" Error in SFibersTP4to1Unpacker: cpar[0]=-100.0" << std::endl;
        return -100;
    }
    else 
    {
        return qdc*511./correctNLC(cpar[0]); 
    }      
        
}


float SFibersRawClusterFinder::correctNLC(float qdc){
    //non-linearity correction, coefficients from PETSYS specs
    
    float p0=8.0;
    float p1=1.04676;
    float p2=1.02734;
    float p3=0.31909;	
    
    if(qdc==-100)
    {
//         std::cout<<" Error in SFibersTP4to1Unpacker:___cp_l[0]=-100.0" << std::endl;
        return -100;
    }
    else
        return (p0*pow(p1, pow(qdc, p2))+p3*qdc-p0); 
}



identifiedFiberDataC convertAddressC(std::vector<std::string> v)
{
    if(v.size() != 4)
    {
        std::cout << "In " << __func__ << std::endl;
        std::cout << "Vector length should be 4!" << std::endl;
        std::abort();
    }
    
    identifiedFiberDataC f;
    f.mod = stoi(v[0]);
    f.lay = stoi(v[1]);
    f.fi = stoi(v[2]);
    f.side = v[3][0];
    
    return f;
}
bool SFibersRawClusterFinder::finalize() { return true; }

std::vector<identifiedFiberDataC> SFibersRawClusterFinder::CommonFibers(SSiPMCluster* TopCluster, SSiPMCluster* BottomCluster)
{
    SSiPMHit * pHit;
    std::vector<std::vector<std::string>> AddressesTop;
    std::vector<std::vector<std::string>> AddressesBottom;
    std::vector<std::vector<std::string>> vec;
    std::vector<identifiedFiberDataC> FiberAddresses;
    std::vector<Int_t> hits;
    Int_t ChannelID = 800;
    
    hits = TopCluster->getHitsArray();
    
    for(int k=0; k<hits.size(); ++k)
    {
        pHit=(SSiPMHit *)catSiPMsHit->getObject(hits[k]);
        pHit->getChannel(ChannelID);
//         std::cout << "chanID" << ChannelID << std::endl;
        SMultiFibersChannel* lc = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, ChannelID));
        vec = lc->vecFiberAssociations;
        AddressesTop.insert(AddressesTop.end(),vec.begin(),vec.end());
    }
    
    hits = BottomCluster->getHitsArray();
    for(int k=0; k<hits.size(); ++k)
    {
        pHit=(SSiPMHit *)catSiPMsHit->getObject(hits[k]);
        pHit->getChannel(ChannelID);
//         std::cout << "chanID" << ChannelID << std::endl;
        SMultiFibersChannel* lc = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, ChannelID));
        vec = lc->vecFiberAssociations;
        AddressesBottom.insert(AddressesBottom.end(),vec.begin(),vec.end());
    }
    
    for(int l=0; l<AddressesBottom.size(); ++l)
    {
       for(int n=0; n<AddressesTop.size(); ++n)
       {
            if(AddressesTop[n][0]==AddressesBottom[l][0] and //module, 
               AddressesTop[n][1]==AddressesBottom[l][1] and //layer,
               AddressesTop[n][2]==AddressesBottom[l][2]) //fiber
            {
                identifiedFiberDataC fib = convertAddressC(AddressesTop[n]);
                FiberAddresses.push_back(fib);
            }
       }
    }
    
    hits.clear();
    AddressesBottom.clear();
    AddressesTop.clear();
    vec.clear();
    
    return FiberAddresses;
}


std::vector <identifiedFiberDataC> SFibersRawClusterFinder::getFibersFromCluster(SSiPMCluster *cluster)
{
    std::vector <Int_t> hits = cluster->getHitsArray();
    int nhits = hits.size();
    
    SMultiFibersChannel* fibLookupChannel;
    
    SSiPMHit* pHit = nullptr;
    int chan = 0;
    
    std::vector <identifiedFiberDataC> addresses;
    std::vector<std::vector<std::string>> vec;
    std::vector<std::string> tmp_str_addr;
    
    for(int i=0; i<nhits; ++i)
    {
        pHit = dynamic_cast<SSiPMHit*>(catSiPMsHit->getObject(hits[i]));
        pHit->getChannel(chan);
        fibLookupChannel = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, chan));
        vec = fibLookupChannel->vecFiberAssociations;
        
        for(int j=0; j<vec.size(); ++j)
        {
            for(int k=0; k<vec[j].size(); ++k)
            {
                tmp_str_addr.push_back(vec[j][k]);
            }
            
            identifiedFiberDataC tmp_fib_addr = convertAddressC(tmp_str_addr);
            addresses.push_back(tmp_fib_addr);
            tmp_str_addr.clear();
        }
    }
    
    return addresses;
}
