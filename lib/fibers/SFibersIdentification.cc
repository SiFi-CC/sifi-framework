#include "SFibersIdentification.h"
#include "SCategory.h"
#include "SLocator.h"
#include "SLookup.h"
#include "SSiPMHit.h"
#include "STP4to1Source.h"
#include "SUnpacker.h"
#include "STask.h"
#include "SiFi.h"
#include "SFibersRaw.h"
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
    
    // get TP calibrator parameters
    pTOFPETCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FibersTOFPETCalibratorPar"));

    if (!pTOFPETCalPar)
    {
        std::cerr << "Parameter container 'STPCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    return true;
}

bool checkIfNeighbours(std::vector<int> fiber_1, std::vector<int> fiber_2)
{
    int mod_1, lay_1, fib_1;
    int mod_2, lay_2, fib_2;

    mod_1 = fiber_1[0];
    lay_1 = fiber_1[1];
    fib_1 = fiber_1[2];
    
    mod_2 = fiber_2[0];
    lay_2 = fiber_2[1];
    fib_2 = fiber_2[2];

    
    if(mod_1 != mod_2) // if two hits were in different modules they are not neighbours
        return false;
    
    if(abs(lay_1 - lay_2) < 2 &&    // if two fibers have neighbouring fiber and layer numbers they are neighbours
       abs(fib_1 - fib_2) < 2)
    {
        return true;
    }
    else
        return false;
}
bool SFibersIdentification::execute()
{ 
    const int N_MODULES = 1;
    
    SLocator loc(3);
    
    std::vector<SSiPMCluster*> ClustersTop[N_MODULES];
    std::vector<SSiPMCluster*> ClustersBottom[N_MODULES];
//     std::vector<fibAddress> FiberAddresses;
    std::vector<SFibersRaw*> fibersRaw;

    
    size_t nClus = catSiPMsCluster->getEntries();
    if(nClus < 2) return true;
    
    Int_t mod;
    char side;
    SSiPMCluster* pCluster;
    
    for (uint j = 0; j < nClus; ++j) // sorting clusters into two groups: top and bottom
    {
        pCluster = (SSiPMCluster *)catSiPMsCluster->getObject(j);
        pCluster->getAddress(mod, side);

        if(side=='r')
            ClustersTop[mod].push_back(pCluster);
        else 
            ClustersBottom[mod].push_back(pCluster);
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
                std::vector<fibAddress> fib = CommonFibers(ClustersTop[mod][l],ClustersBottom[mod][k]);
//                 FiberAddresses.insert(FiberAddresses.end(),fib.begin(),fib.end());
                
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
    std::vector<std::vector<std::vector<int>>> fibersInClusterPairs; //for each clusterPair (in clusterPairs), store a vector of adressess=3-vectors=m,l,f of all involved fibers; TODO change the innermost vector to fibAdress.
    
    int topClusID, botClusID;
    int prevTopClusID, prevBotClusID;
    topClusID=-1;
    botClusID=-1;
    prevTopClusID=-1;
    prevBotClusID=-1;
    int clusterFiberMult = 1;

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
            std::cout << " topClusID | botClusID: " <<  topClusID << " " << botClusID << " fibAdress: " << mod << " " << lay << " " << fi << std::endl;
            
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
            int topClusID, botClusID;
            fibersInClusterPairs.push_back({});
            std::cout << "cluster pair: " <<clusterPairs[i][0] << " " << clusterPairs[i][1] << " f mult: " << clusterPairs[i][2] <<  std::endl;
            for(int j=0; j<fibersRaw.size(); j++){
                fibersRaw[j]->getClusters(topClusID, botClusID);
                if(topClusID == clusterPairs[i][0] && botClusID == clusterPairs[i][1]){
                    fibersRaw[j]->getAddress(mod,lay,fi);
                    fibersInClusterPairs[i].push_back({mod,lay,fi});
                }
            }
        }
    }

    fiberClusterHitType fibClusType; 
    int topIter = 0;
    int botIter = 0;
    std::vector<std::vector<int>> neighbours;
    if(clusterPairs.size()==1){
        if(clusterPairs[0][2]==1){ // check if multiplicity of the cluster = 1
            fibClusType=uniqueClusterUniqueFiber;
            std::cout << "label: unique, single fiber" << std::endl;
        }
        else{
            std::cout << "label: unique, mult: " << clusterPairs[0][2] << std::endl;
            fibClusType=uniqueClusterMultipleFiber;
            fibersRaw[0]->setFiberMultInCluster(clusterPairs[0][2]);
//             //here add checking if neighbours
//             neighbours.push_back(fibersInClusterPairs[0][0]);
//             for(int k=0;k<neighbours.size(); k++){
//                 if(checkIfNeighbours(fibersInClusterPairs[0][0],fibersInClusterPairs[0][k])){
//                     neighbours.push_back(fibersInClusterPairs[0][k]); //do it so that each new fiber is checked against all that are already in the neighbour vector. Then, plot summarise how many of not-neighbours do we have.
//                 }
//             }
            //std::cout << "checkIfNeighbours" << checkIfNeighbours(fibersInClusterPairs[0][0],fibersInClusterPairs[0][1]) << std::endl;
        }
    fibersRaw[0]->setFiberClusterLabel(fibClusType);
    } 
    else if (clusterPairs.size()>1){
        for(int i=0;i<clusterPairs.size(); i++){
            for(int j=0;j<clusterPairs.size(); j++){
                if(clusterPairs[i][0] == clusterPairs[j][0]) topIter++;
                if(clusterPairs[i][1] == clusterPairs[j][1]) botIter++;
            }
            if(topIter == 1 && botIter == 1) {
                std::cout << "label: unique cluster pair(topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=uniqueClusterMultipleFiber;
                fibersRaw[i]->setFiberMultInCluster(clusterPairs[i][2]);
                //here also add checking if neighbours
            }
            else if(topIter == 2 && botIter == 1)  {
                std::cout << "label: semi-unique BOT cluster pair(topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=bottomUniqueCluster;
            }
            else if(topIter == 1 && botIter == 2)  {
                std::cout << "label: semi-unique TOP cluster pair(topITER, botITER) = "     << topIter << " " << botIter << std::endl;
                fibClusType=topUniqueCluster;
            }
            else {
                std::cout << "label: ambiguous cluster pair: (topITER, botITER) = " << topIter << " " << botIter << std::endl;
                fibClusType=ambiguousCluster;
            }
            fibersRaw[i]->setFiberClusterLabel(fibClusType); 
            topIter = 0;
            botIter = 0;
        }
    }
    else std::cerr << "clusterPairs.size() <= 0. Incorrect." << std::endl;
    for(int i=0;i<clusterPairs.size(); i++){
       
    }
    
    
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



float SFibersIdentification::alignQDC(fibAddress address, float qdc)
{
    SFibersChannel chan;
    chan.m = address.mod;
    chan.l = address.lay;
    chan.s = address.fi;
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


float SFibersIdentification::correctNLC(float qdc){
    //non-linearity correction, coefficients from PETSYS specs
    
    float p0=8.0;
    float p1=1.04676;
    float p2=1.02734;
    float p3=0.31909;	
    
    if(qdc==-100)
    {
        std::cout<<" Error in SFibersTP4to1Unpacker: cp_l[0]=-100.0" << std::endl;
        return -100;
    }
    else
        return (p0*pow(p1, pow(qdc, p2))+p3*qdc-p0); 
}



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
bool SFibersIdentification::finalize() { return true; }

std::vector<fibAddress> SFibersIdentification::CommonFibers(SSiPMCluster* TopCluster, SSiPMCluster* BottomCluster)
{
    SSiPMHit * pHit;
    std::vector<std::vector<std::string>> AddressesTop;
    std::vector<std::vector<std::string>> AddressesBottom;
    std::vector<std::vector<std::string>> vec;
    std::vector<fibAddress> FiberAddresses;
    std::vector<Int_t> hits;
    Int_t ChannelID = 800;
    
    hits = TopCluster->getHitsArray();
    
    for(int k=0; k<hits.size(); ++k)
    {
        pHit=(SSiPMHit *)catSiPMsHit->getObject(hits[k]);
        pHit->getChannel(ChannelID);
        std::cout << "chanID" << ChannelID << std::endl;
        SMultiFibersChannel* lc = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, ChannelID));
        vec = lc->vecFiberAssociations;
        AddressesTop.insert(AddressesTop.end(),vec.begin(),vec.end());
    }
    
    hits = BottomCluster->getHitsArray();
    for(int k=0; k<hits.size(); ++k)
    {
        pHit=(SSiPMHit *)catSiPMsHit->getObject(hits[k]);
        pHit->getChannel(ChannelID);
        std::cout << "chanID" << ChannelID << std::endl;
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
                fibAddress fib = convertAddress(AddressesTop[n]);
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


std::vector <fibAddress> SFibersIdentification::getFibersFromCluster(SSiPMCluster *cluster)
{
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
        
        for(int j=0; j<vec.size(); ++j)
        {
            for(int k=0; k<vec[j].size(); ++k)
            {
                tmp_str_addr.push_back(vec[j][k]);
            }
            
            fibAddress tmp_fib_addr = convertAddress(tmp_str_addr);
            addresses.push_back(tmp_fib_addr);
            tmp_str_addr.clear();
        }
    }
    
    return addresses;
}
