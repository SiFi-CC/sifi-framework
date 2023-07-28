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
    
    // get TP calibrator parameters
    pTOFPETCalPar = dynamic_cast<SCalContainer<6>*>(pm()->getCalContainer("FibersTOFPETCalibratorPar"));

    if (!pTOFPETCalPar)
    {
        std::cerr << "Parameter container 'STPCalibratorPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    return true;
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
                
//                 if(FiberAddresses.size()==2)
//                 {
//                     int distLay = FiberAddresses[0].iLay-FiberAddresses[1].iLay;
//                     int distFib = FiberAddresses[0].iFib-FiberAddresses[1].iFib;
//                     dist->Fill(distFib,distLay);
//                 }
                
//                 FiberBottom[k]=FiberBottom[k]+FiberAddresses.size();
//                 FiberTop[l]=FiberTop[l]+FiberAddresses.size();
                
//                 FiberAddresses.clear();
            }
        }
    }

    //TODO add selection of valid events below!
    
    for(int f=0; f<fibersRaw.size(); ++f)
    {
        if(fibersRaw.size() != 1)   // only events in which single fiber is uniquely identified are valid TEMPORARY
            continue;
        
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

bool SFibersIdentification::finalize() { return true; }

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
        SMultiFibersChannel* lc = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, ChannelID));
        vec = lc->vecFiberAssociations;
        AddressesTop.insert(AddressesTop.end(),vec.begin(),vec.end());
    }
    
    hits = BottomCluster->getHitsArray();
    for(int k=0; k<hits.size(); ++k)
    {
        pHit=(SSiPMHit *)catSiPMsHit->getObject(hits[k]);
        pHit->getChannel(ChannelID);
        SMultiFibersChannel* lc = dynamic_cast<SMultiFibersChannel*>(fibLookup->getAddress(0x1000, ChannelID));
        vec = lc->vecFiberAssociations;
        AddressesBottom.insert(AddressesBottom.end(),vec.begin(),vec.end());
    }

    for(int l=0; l<AddressesBottom.size(); ++l)
    {
       for(int n=0; n<AddressesTop.size(); ++n)
       {
            if(AddressesTop[n][0]==AddressesBottom[l][0] and 
               AddressesTop[n][1]==AddressesBottom[l][1] and 
               AddressesTop[n][2]==AddressesBottom[l][2])
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
