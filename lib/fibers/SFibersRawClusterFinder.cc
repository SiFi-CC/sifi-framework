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
    
    catSiPMsHit->print(); // BUG why print shows empty category?
    
    catSiPMsCluster = sifi()->getCategory(SCategory::CatSiPMClus);
   
    if(!catSiPMsCluster)
    {
       std::cerr << "No CatSiPMClus category!" << std::endl;
    }
    catSiPMsCluster->print(); // BUG why print shows empty category?
    
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
    
//     // get TP calibrator parameters
//     pTOFPETCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FibersTOFPETCalibratorPar"));
// 
//     if (!pTOFPETCalPar)
//     {
//         std::cerr << "Parameter container 'STPCalibratorPar' was not obtained!" << std::endl;
//         exit(EXIT_FAILURE);
//     }

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
    
    if(mod_1 != mod_2){ // if two hits are in different modules they are not neighbours
        std::cout << "modules not equal!" << std::endl;
        return false;
    }
    
    if(abs(lay_1 - lay_2) < 2 &&    // if two fibers have neighbouring fiber and layer numbers they are neighbours
       abs(fib_1 - fib_2) < 2)
    {
        return true;
    }
    else{
        return false;
        }
}

std::vector<int> createFiberClusters(std::vector<std::vector<identifiedFiberDataC>> fibsInClusterPairs)
{
    std::vector<int> nFibClustersInPairs; //returned from the function, length: n_pairs
    std::vector<std::vector<std::vector<identifiedFiberDataC>>> fibClustersInPairs;
    
        for (int i = 0; i < fibsInClusterPairs.size(); i++){ //iterate over all cluster pairs
            std::vector<identifiedFiberDataC> unassignedFibers;
            std::vector<std::vector<identifiedFiberDataC>> fibClusters;
            std::vector<identifiedFiberDataC>::iterator it;
            unassignedFibers = fibsInClusterPairs[i];
            std::vector<bool> isUnassigned;
            for(int j=0; j<unassignedFibers.size(); j++){
                isUnassigned.push_back(1);
            }
            int allFib_size = unassignedFibers.size();
            int clusterID = 0;
            int fibInClusterCounter = 0;
            int prev_fibInClusterCounter = -1;
            fibClusters.push_back({});
            fibClusters[clusterID].push_back(unassignedFibers.front());
            fibInClusterCounter = 1;
            isUnassigned[0] = 0;
            while(std::accumulate(isUnassigned.begin(), isUnassigned.end(), 0)){
                while(prev_fibInClusterCounter != fibInClusterCounter){
                    prev_fibInClusterCounter = fibClusters[clusterID].size();
                    for(int m=0; m<unassignedFibers.size(); m++){
                        for(int n=0; n<fibClusters[clusterID].size(); n++){
                            if(isUnassigned[m]){
                                if(checkIfNeighboursC(unassignedFibers[m], fibClusters[clusterID][n])){
                                    fibClusters[clusterID].push_back(unassignedFibers[m]);
                                    fibInClusterCounter=fibClusters[clusterID].size();
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
                            clusterID++;
                            break;
                        }
                    }              
                }
            }
            nFibClustersInPairs.push_back(clusterID+1);    
        }
    return nFibClustersInPairs;
    }

std::vector<float> getFirstFiber(std::vector<identifiedFiberDataC> fibersInPairOfClusters){
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
    int min_layer = 0; 
    int layer = 0;
    min_layer  = fibersInPairOfClusters[0].lay;
    for(int i=0; i<n_fibers; i++){
        if(fibersInPairOfClusters[i].mod == 0){
            layer = fibersInPairOfClusters[i].lay;
            if(layer < min_layer){
                min_layer = layer;
            }
            qavg = sqrt(fibersInPairOfClusters[i].QDCL*fibersInPairOfClusters[i].QDCR);
            qdc_sum += qavg;
            ffi+=fibersInPairOfClusters[i].fi*qavg;
        }
        
        //else std::cerr << "In SFibersRawClusterFinder module != 0. Check all involved fiber addresses. If using absorber, this function needs to be edited" << std::endl;
    }
    flay = min_layer;
    ffi = ffi/qdc_sum;
    clusterCenterAddress.push_back(fmod);
    clusterCenterAddress.push_back(flay);
    clusterCenterAddress.push_back(ffi);
    return clusterCenterAddress;
}

identifiedFiberDataC getClusterPosition(std::vector<identifiedFiberDataC> fibersInPairOfClusters, std::map<int, float> topClusterAlignedQDCs, std::map<int, float> botClusterAlignedQDCs){
    Float_t qdc_l, qdc_r;
    qdc_l=0;
    qdc_r=0;
    Float_t fmod, flay, ffi;

    qdc_r = topClusterAlignedQDCs[fibersInPairOfClusters[0].topClusID]; //taking first element because for all fibers, cluster pair IDs is the same (as this is a vector of fibers within the same cluster pair)
    qdc_l = botClusterAlignedQDCs[fibersInPairOfClusters[0].botClusID];
    
    std::vector<float> clusterCenterAddress;
    clusterCenterAddress = getFirstFiber(fibersInPairOfClusters);
    fmod = clusterCenterAddress[0];
    flay = clusterCenterAddress[1];
    ffi = clusterCenterAddress[2];
    identifiedFiberDataC clusterPosition; //qdc_l, qdc_r, address
    clusterPosition.QDCL = qdc_l;
    clusterPosition.QDCR = qdc_r;
    clusterPosition.mod = round(fmod);
    clusterPosition.lay = round(flay);
    clusterPosition.fi = round(ffi);
    return clusterPosition;
}

std::vector<int> getSiPMsFromFiber(int m, int l, int f){
    SCalContainer<6>* pFiberToSiPMPar;
    pFiberToSiPMPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FiberToSiPMPar"));
    if (!pFiberToSiPMPar){
        std::cerr << "Parameter container 'FiberToSiPMPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<int> sipmIDs;
    SFibersChannel chan;
    chan.m = m;
    chan.l = l;
    chan.s = f;
    chan.side = 'l';
    
    auto _cpar = pFiberToSiPMPar->getPar(&chan);
    auto&& cpar = *_cpar;
    
    for(int i=0; i<6; i++){
        sipmIDs.push_back(cpar[i]);
    }
    
    return sipmIDs;
}

bool SFibersRawClusterFinder::execute()
{ 
    const int N_MODULES = 1; //TODO to musi być brane z pliku params!
    SLocator loc(3);
    
    std::vector<SSiPMCluster*> ClustersTop[N_MODULES];
    std::vector<SSiPMCluster*> ClustersBottom[N_MODULES];
    std::vector<SSiPMHit*> SiPMsTop[N_MODULES];
    std::vector<SSiPMHit*> SiPMsBottom[N_MODULES];
    std::vector<std::unique_ptr<SFibersRaw>> fibersRaw;
    std::vector<std::unique_ptr<SFibersRawCluster>> fibersRawCluster;
    
    size_t nClus = catSiPMsCluster->getEntries();
    if(nClus < 2) return true;
    
    Int_t mod = 0;
    char side = ' ';
    int tcID = 0, bcID = 0;
    
    SSiPMCluster* pCluster = nullptr;
    std::map<int,float> topClusterAlignedQDCs; //sum of aligned qdcs of contributing SiPMs
    std::map<int,float> botClusterAlignedQDCs; //sum of aligned qdcs of contributing SiPMs
    std::vector<int> sipmsCoupledToFiber;
    
    // take clusters from the SSiPMCluster category, sort them into two groups: top and bottom; fill topClusterAlignedQDCs and botClusterAlignedQDCs
    for (uint j = 0; j < nClus; ++j) 
    {
        pCluster = (SSiPMCluster *)catSiPMsCluster->getObject(j);
        pCluster->getAddress(mod, side);

        if(side=='r'){
            ClustersTop[mod].push_back(pCluster);
            pCluster->getID(tcID);
            topClusterAlignedQDCs.insert({tcID, pCluster->getAlignedQDC()});
        }
        else {
            ClustersBottom[mod].push_back(pCluster);
            pCluster->getID(bcID);
            botClusterAlignedQDCs.insert({bcID, pCluster->getAlignedQDC()});
        }
    }
    
    size_t nSiPMHit = catSiPMsHit->getEntries();
    
    Int_t s_mod = 0, s_lay = 0, s_el = 0;
    char s_side = ' ';
    SSiPMHit* pHit = nullptr;
    
    for (uint j = 0; j < nSiPMHit; ++j){ // Take SiPMs from the SSiPMHit category, sort them into two groups: top and bottom
        pHit = (SSiPMHit *)catSiPMsHit->getObject(j);
        pHit->getAddress(s_mod, s_lay, s_el, s_side);
        if(s_side=='r')
            SiPMsTop[s_mod].push_back(pHit);
        else 
            SiPMsBottom[s_mod].push_back(pHit);
    }

    //Fill the fibersRaw vector: find common fibers for each top-bottom cluster pair
    for(int m = 0; m < N_MODULES; ++m)  
    {
        if(ClustersBottom[m].size() == 0 || ClustersTop[m].size() == 0) return true;
        
//         std::unique_ptr<int[]> FiberBottom(new int[ClustersBottom[mod].size()]);
//         std::unique_ptr<int[]> FiberTop(new int[ClustersTop[mod].size()]);
//         
//         for(int k=0; k<ClustersBottom[mod].size(); ++k)
//         {
//             FiberBottom[k]=0;
//         }
//         
//         for(int l=0; l<ClustersTop[mod].size(); ++l)
//         {
//             FiberTop[l]=0;
//         }
        
        for(int k=0; k<ClustersBottom[mod].size(); ++k)
        {

            for(int l=0; l<ClustersTop[mod].size(); ++l)
            {
                std::vector<identifiedFiberDataC> fib = CommonFibers(ClustersTop[mod][l],ClustersBottom[mod][k]);
                for(int f=0; f<fib.size(); ++f)
                {
                    std::unique_ptr<SFibersRaw> fr = std::make_unique<SFibersRaw>();
                    fr->setAddress(fib[f].mod, fib[f].lay, fib[f].fi);
//                     fr->setQDCL(alignQDC(fib[f], ClustersBottom[mod][k]->getQDC())); //changed to SiPM gain alignment
//                     fr->setQDCR(alignQDC(fib[f], ClustersTop[mod][l]->getQDC())); //changed to SiPM gain alignment
                    sipmsCoupledToFiber = getSiPMsFromFiber(fib[f].mod, fib[f].lay, fib[f].fi); 
//                     std::cout << sipmsCoupledToFiber[0] << " " <<sipmsCoupledToFiber[1] << " " <<sipmsCoupledToFiber[2] << " " <<sipmsCoupledToFiber[3] << " " <<sipmsCoupledToFiber[4] << " " <<sipmsCoupledToFiber[5] << std::endl;
                    
                    for(int i = 0; i< SiPMsBottom[s_mod].size(); i++){
                        SiPMsBottom[s_mod][i]->getAddress(s_mod, s_lay, s_el, s_side);
                        if(s_mod == sipmsCoupledToFiber[0] && s_lay == sipmsCoupledToFiber[1] && s_el == sipmsCoupledToFiber[2]) {
                            fr->setQDCL(SiPMsBottom[s_mod][i]->getAlignedQDC());
                        }
                    }
                    for(int i = 0; i< SiPMsTop[s_mod].size(); i++){
                        SiPMsTop[s_mod][i]->getAddress(s_mod, s_lay, s_el, s_side);
                        if(s_mod == sipmsCoupledToFiber[3] && s_lay == sipmsCoupledToFiber[4] && s_el == sipmsCoupledToFiber[5]) {
                            fr->setQDCR(SiPMsTop[s_mod][i]->getAlignedQDC());
                        }
                    }
                    fr->setTimeL(ClustersBottom[mod][k]->getTime());
                    fr->setTimeR(ClustersTop[mod][l]->getTime());
                    Int_t id_t, id_b;
                    ClustersTop[mod][l]->getID(id_t);
                    ClustersBottom[mod][k]->getID(id_b);
                    fr->setClusters(id_t, id_b);
                    
                    fibersRaw.push_back(std::move(fr));
//                     fr->Clear();
                }
                
                fib.clear();
                
            }
        }
    }
    
    
    //print Clusters and SiPMs content (debug only)
    
//     std::cout << "Bottom" << std::endl;
//     for(int l=0;l<ClustersBottom[0].size(); l++){
//         ClustersBottom[0][l]->print();
//     }
//     for(int m=0; m<SiPMsBottom[0].size(); m++){
//         SiPMsBottom[0][m]->print();
//     }
//     
//     std::cout << "Top" << std::endl;
//     for(int l=0;l<ClustersTop[0].size(); l++){
//         ClustersTop[0][l]->print();
//     }
//     for(int m=0; m<SiPMsTop[0].size(); m++){
//         SiPMsTop[0][m]->print();
//     }


    //print fibersRaw contents (debug only)
//     for(int f=0; f< fibersRaw.size(); f++){
//         int mod, lay, fib, tID, bID;
//         fibersRaw[f]->getAddress(mod, lay, fib);
//         fibersRaw[f]->getClusters(tID, bID);
//         std::cout << "fibersRawContents: " << mod << " " << lay<< " " << fib  << " " << fibersRaw[f]->getQDCL()<< " " << fibersRaw[f]->getQDCR() << " " << fibersRaw[f]->getTimeL()<< " " << fibersRaw[f]->getTimeR() << " clusters: " << tID << " " << bID <<std::endl;
//     }

    std::vector<std::vector<int>> clusterPairs; //vectors of length 3: topClusterID, botClusterID, multiplicity 
    std::vector<std::vector<identifiedFiberDataC>> fibersInClusterPairs; //for each clusterPair (in clusterPairs), store a vector of adressess=m,l,f of all involved fibers
    
    int topClusID, botClusID;
    int prevTopClusID, prevBotClusID;
    topClusID=-1;
    botClusID=-1;
    prevTopClusID=-1;
    prevBotClusID=-1;
    int clusterFiberMult = -1;

// create vector of cluster pairs and fill fibersInClusterPairs
    if(!fibersRaw.empty()) {
        fibersRaw[0]->getClusters(topClusID, botClusID);
        prevTopClusID = topClusID;
        prevBotClusID = botClusID;
        clusterFiberMult = 1;
        for(int f=0; f<fibersRaw.size(); f++){
            int mod = 0, lay = 0, fi = 0;
            char side = ' ';
            prevTopClusID = topClusID;
            prevBotClusID = botClusID;
            fibersRaw[f]->getClusters(topClusID, botClusID);
            fibersRaw[f]->getAddress(mod,lay,fi);
//             std::cout << "f: " << f <<  " topClusID | botClusID: " <<  topClusID << " " << botClusID << " fibAdress: " << mod << " " << lay << " " << fi << std::endl;

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
            int mod = 0, lay = 0, fi = 0;
            char side = ' ';
            float qdcl = 0, qdcr = 0;
            Double_t timel = 0,timer = 0;
            int topClusID = 0, botClusID = 0;
            fibersInClusterPairs.push_back({});
//             std::cout << i << "-th cluster pair: " <<clusterPairs[i][0] << " " << clusterPairs[i][1] << " f mult: " << clusterPairs[i][2] <<  std::endl;
            for(int j=0; j<fibersRaw.size(); j++){
                fibersRaw[j]->getClusters(topClusID, botClusID);
               
                if(topClusID == clusterPairs[i][0] && botClusID == clusterPairs[i][1]){
                    fibersRaw[j]->getAddress(mod,lay,fi);
                    qdcl=fibersRaw[j]->getQDCL();
                    qdcr=fibersRaw[j]->getQDCR();
                    timel=fibersRaw[j]->getTimeL();
                    timer=fibersRaw[j]->getTimeR();
                    identifiedFiberDataC f;
                    f.mod = mod;
                    f.lay = lay;
                    f.fi = fi;
                    f.side = side;
                    f.timeL = timel;
                    f.timeR = timer;
                    f.QDCL = qdcl;
                    f.QDCR = qdcr;
                    f.topClusID = topClusID;
                    f.botClusID = botClusID;
                    fibersInClusterPairs[i].push_back(f);
                }
            }
        }
    }
    else {
//         std::cerr << "ERROR in SFibersRawClusterFinder: vector fibersRaw empty! " << std::endl; //TODO what's the meaning of this error, should it be taken care of?
    }

//  labeling event classes
    fiberClusterHitTypeC fibClusType; 
    int topIter = 0;
    int botIter = 0;
    std::vector<int> foundFiberClusters;
    
    if (clusterPairs.size()>=1){
        for(int i=0;i<clusterPairs.size(); i++){
            for(int j=0;j<clusterPairs.size(); j++){
                if(clusterPairs[i][0] == clusterPairs[j][0]) topIter++;
                if(clusterPairs[i][1] == clusterPairs[j][1]) botIter++;
            }
            if(topIter == 1 && botIter == 1){
                if(clusterPairs[i][2] == 1){
//                     std::cout << "label: unique, SINGLE FIBER" << std::endl;
                    fibClusType=uniqueClusterUniqueFiberC; // in this case, simply fill the SFibersRawCluster with SFibersRaw contents (no clusters created, only single-fiber-events)
                    std::unique_ptr<SFibersRawCluster> frc = std::make_unique<SFibersRawCluster>();
                    frc->setAddress(fibersInClusterPairs[i][0].mod, fibersInClusterPairs[i][0].lay, fibersInClusterPairs[i][0].fi);
                    fibersInClusterPairs[i][0].side = 'l';
                    frc->setQDCL(fibersInClusterPairs[i][0].QDCL);
                    frc->setTimeL(fibersInClusterPairs[i][0].timeL);
                    fibersInClusterPairs[i][0].side = 'r';
                    frc->setQDCR(fibersInClusterPairs[i][0].QDCR);
                    frc->setTimeR(fibersInClusterPairs[i][0].timeR);
                    frc->setFiberClusterLabel(fibClusType);
                    frc->setFiberMultInCluster(clusterPairs[i][2]);
                    frc->setNFibersClusters(1);
                    fibersRawCluster.push_back(std::move(frc));
//                     frc->Clear();
                }
                else {
                //check if neighbours
//                 std::cout << "label: unique, mult fibers" << std::endl;
                identifiedFiberDataC fibClusterData;
                foundFiberClusters = createFiberClusters(fibersInClusterPairs);
                fibClusType=uniqueClusterMultipleFiberC;
                std::unique_ptr<SFibersRawCluster> frc = std::make_unique<SFibersRawCluster>();
                if(foundFiberClusters[i] == 1){
                    fibClusterData = getClusterPosition(fibersInClusterPairs[i], topClusterAlignedQDCs, botClusterAlignedQDCs);
                    frc->setAddress(fibClusterData.mod, fibClusterData.lay, fibClusterData.fi);
                    fibClusterData.side = 'l';
                    frc->setQDCL(fibClusterData.QDCL);
                    frc->setTimeL(fibersInClusterPairs[i][0].timeL);
                    fibClusterData.side = 'r';
                    frc->setQDCR(fibClusterData.QDCR);
                    frc->setTimeR(fibersInClusterPairs[i][0].timeR);
                    frc->setFiberClusterLabel(fibClusType);
                    frc->setFiberMultInCluster(clusterPairs[i][2]);
                    frc->setNFibersClusters(foundFiberClusters[i]); //if this is needed, replace the if condition below with "fibersRawCluster.push_back(frc);"
                    fibersRawCluster.push_back(std::move(frc));
//                     frc->Clear();
                }
                else if(foundFiberClusters[i] > 1){ //BUG is this correct? 
                    frc->setAddress(1,0,0);         //BUG this cannot be hardcoded like this! 1 will be a valid module!
                    frc->setQDCL(-100);
                    frc->setTimeL(-100);
                    frc->setQDCR(-100);
                    frc->setTimeR(-100);
                    frc->setFiberClusterLabel(fibClusType); // BUG what fiber cluster type is this?
                    frc->setFiberMultInCluster(-1);
                    frc->setNFibersClusters(foundFiberClusters[i]);
                    fibersRawCluster.push_back(std::move(frc));
//                     frc->Clear();
                }
                else std::cerr << "Error in float SFibersRawClusterFinder: foundFiberClusters[i] < 0" << std::endl;
                
                

                }
            }
            else if(topIter == 2 && botIter == 1)  {
//                 std::cout << "label: semi-unique BOT cluster pair(topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=bottomUniqueClusterC;
                std::unique_ptr<SFibersRawCluster> frc = std::make_unique<SFibersRawCluster>();
                frc->setAddress(1,0,0);
                frc->setQDCL(-100);
                frc->setTimeL(-100);
                frc->setQDCR(-100);
                frc->setTimeR(-100);
                frc->setFiberClusterLabel(fibClusType);
                frc->setFiberMultInCluster(-1);
                frc->setNFibersClusters(-1);
                fibersRawCluster.push_back(std::move(frc));
//                 frc->Clear();
            }
            else if(topIter == 1 && botIter == 2)  {
//                 std::cout << "label: semi-unique TOP cluster pair(topITER, botITER) = "     << topIter << " " << botIter << std::endl;
                fibClusType=topUniqueClusterC;
                std::unique_ptr<SFibersRawCluster> frc = std::make_unique<SFibersRawCluster>();
                frc->setAddress(1,0,0);
                frc->setQDCL(-100);
                frc->setTimeL(-100);
                frc->setQDCR(-100);
                frc->setTimeR(-100);
                frc->setFiberClusterLabel(fibClusType);
                frc->setFiberMultInCluster(-1);
                frc->setNFibersClusters(-1);
                fibersRawCluster.push_back(std::move(frc));
//                 frc->Clear();
            }
            else {
//                 std::cout << "label: ambiguous cluster pair: (topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=ambiguousClusterC;
                std::unique_ptr<SFibersRawCluster> frc = std::make_unique<SFibersRawCluster>();
                frc->setAddress(1,0,0);
                frc->setQDCL(-100);
                frc->setTimeL(-100);
                frc->setQDCR(-100);
                frc->setTimeR(-100);
                frc->setFiberClusterLabel(fibClusType);
                frc->setFiberMultInCluster(-1);
                frc->setNFibersClusters(-1);
                fibersRawCluster.push_back(std::move(frc));
//                 frc->Clear();
            }
            topIter = 0;
            botIter = 0;
        }
    }
    else {
//         std::cerr << "clusterPairs.size() <= 0. Incorrect." << std::endl; //TODO what's the meaning of this error, should it be taken care of?
    }
    
    //SFibersRawCluster filling
    for(int f=0; f<fibersRawCluster.size(); ++f) //size of fibersRawCluster has to be correct for this to work properly (it is currently the size of fibersRaw)
    {
        int mod, lay, fib;
        fibersRawCluster[f]->getAddress(mod, lay, fib);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersRawCluster *pRawClus = reinterpret_cast<SFibersRawCluster*>(catFibersRawClus->getSlot(loc));
        pRawClus = new (pRawClus) SFibersRawCluster(*fibersRawCluster[f]);

//         std::cout << "fibersRawClusterContents: " << mod << " " << lay<< " " << fib  << " " << fibersRawCluster[f]->getQDCL()<< " " << fibersRawCluster[f]->getQDCR() << " " << fibersRawCluster[f]->getTimeL()<< " " << fibersRawCluster[f]->getTimeR()<<std::endl;
    }
    
    //SFibersRaw filling - only for debugging
    for(int f=0; f<fibersRaw.size(); ++f)
    {
//         if(fibersRaw.size() != 1)   // only events in which single fiber is uniquely identified are valid TEMPORARY
//             continue;

        int mod, lay, fib;
        fibersRaw[f]->getAddress(mod, lay, fib);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = fib;

        SFibersRaw *pRaw = reinterpret_cast<SFibersRaw*>(catFibersRaw->getSlot(loc));
        pRaw = new (pRaw) SFibersRaw(*fibersRaw[f]);
    }

    return true;
}

// float SFibersRawClusterFinder::alignQDC(identifiedFiberDataC address, float qdc)
// {
//     SFibersChannel chan;
//     chan.m = address.mod;
//     chan.l = address.lay;
//     chan.s = address.fi;
//     chan.side = address.side;
//     
//     auto _cpar = pTOFPETCalPar->getPar(&chan);
//     auto&& cpar = *_cpar;
//     
//     if(qdc==-100)
//     {
//         std::cout<<" Error in SFibersTP4to1Unpacker: cpar[0]=-100.0" << std::endl;
//         return -100;
//     }
//     else 
//     {
//         return qdc*511./correctNLC(cpar[0]); 
//     }      
//         
// }


// float SFibersRawClusterFinder::correctNLC(float qdc){
//     //non-linearity correction, coefficients from PETSYS specs
//     
//     float p0=8.0;
//     float p1=1.04676;
//     float p2=1.02734;
//     float p3=0.31909;	
//     
//     if(qdc==-100)
//     {
// //         std::cout<<" Error in SFibersTP4to1Unpacker:___cp_l[0]=-100.0" << std::endl;
//         return -100;
//     }
//     else
//         return (p0*pow(p1, pow(qdc, p2))+p3*qdc-p0); 
// }



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
bool SFibersRawClusterFinder::finalize() { 
    return true; 
}

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
        if(ChannelID == 800 || ChannelID ==801) continue;
        SMultiFibersChannel* lc = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, ChannelID));
        vec = lc->vecFiberAssociations;
        AddressesTop.insert(AddressesTop.end(),vec.begin(),vec.end());
    }
    
    hits = BottomCluster->getHitsArray();
    for(int k=0; k<hits.size(); ++k)
    {
        pHit=(SSiPMHit *)catSiPMsHit->getObject(hits[k]);
        pHit->getChannel(ChannelID);
        if(ChannelID == 800 || ChannelID == 801) continue;
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
