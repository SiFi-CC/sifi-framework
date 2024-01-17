#include "SSimSource.h"
#include "SUnpacker.h"
#include "SSimImporter.h"

SSimSource::SSimSource() : SRootSource("Events"), subevent(0)
{
    fMCPosition_source = new TVector3;
    fMCDirection_source = new TVector3;
    fMCComptonPosition = new TVector3;
    fMCDirection_scatter = new TVector3;
    fMCPosition_e = new std::vector<TVector3>;
    fMCInteractions_e = new std::vector<Int_t>;
    fMCPosition_p = new std::vector<TVector3>;
    fMCInteractions_p = new std::vector<Int_t>;
    fMCEnergyDeps_e = new std::vector<Double_t>;
    fMCEnergyDeps_p = new std::vector<Double_t>;
    vFibreData = new std::vector<FibreData>;
    vSiPMData = new std::vector<SiPMData>;

    chain->SetBranchAddress("EventNumber",&fEventNumber);
    chain->SetBranchAddress("MCSimulatedEventType",&fMCSimulatedEventType);
    chain->SetBranchAddress("MCEnergyPrimary",&fMCEnergyPrimary);
    chain->SetBranchAddress("MCEnergy_e",&fMCEnergy_e);
    chain->SetBranchAddress("MCEnergy_p",&fMCEnergy_p);
    chain->SetBranchAddress("MCPosition_source",&fMCPosition_source);
    chain->SetBranchAddress("MCDirection_source",&fMCDirection_source);
    chain->SetBranchAddress("MCComptonPosition",&fMCComptonPosition);
    chain->SetBranchAddress("MCDirection_scatter",&fMCDirection_scatter);
    chain->SetBranchAddress("MCPosition_e",&fMCPosition_e);
    chain->SetBranchAddress("MCInteractions_e",&fMCInteractions_e);
    chain->SetBranchAddress("MCPosition_p",&fMCPosition_p);
    chain->SetBranchAddress("MCInteractions_p",&fMCInteractions_p);
    chain->SetBranchAddress("MCEnergyDeps_e", &fMCEnergyDeps_e);
    chain->SetBranchAddress("MCEnergyDeps_p", &fMCEnergyDeps_p);
    chain->SetBranchAddress("FibreData",&vFibreData);
    chain->SetBranchAddress("SiPMData",&vSiPMData);
}

bool SSimSource::open()
{
    if (unpackers.size() == 0) return false;

    chain->GetEntry(0);

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();

        bool res = unpackers[subevent]->init();
        if (!res)
        {
            printf("Forced unpacker %#x not initalized\n", subevent);
            abort();
        }
    }
    else
    {
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
        {
            bool res = iter->second->init();
            if (!res)
            {
                printf("Unpacker %#x not initalized\n", iter->first);
                abort();
            }
        }
    }
    return true;
}

bool SSimSource::close()
{
    if (subevent != 0x0000)
    {
        if (unpackers[subevent])
            unpackers[subevent]->finalize();
        else
            abort();
    }
    else
    {
        std::map<uint16_t, SUnpacker*>::iterator iter = unpackers.begin();
        for (; iter != unpackers.end(); ++iter)
            iter->second->finalize();
    }
    return true;
}

bool SSimSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;

    long ce = getCurrentEvent();
    chain->GetEntry(ce);

    if (subevent == 0x0000)
    {
        if (!unpackers[subevent]) abort();
        SSimImporter* unp = dynamic_cast<SSimImporter*>(unpackers[subevent]);
        if (unp) unp->execute(0, 0, subevent, vSiPMData, 1);
    }
    else //TODO implement
    {
//         for (const auto& u : unpackers)
//         {
//             SSimImporter* unp = dynamic_cast<SSimImporter*>(u.second);
//             for (auto& c : counts)
//             {
//                 int fiber_id = c.first % sipm_fold;
//                 std::map<int, TVector3>::iterator iter;
//
//                 if ((iter = fPxPosScin->find(fiber_id)) != fPxPosScin->end())
//                 {
//                     tree.pos = iter->second;
//                     tree.type = SFibersCalSim::InteractionType::SCINT;
//                 }
//                 else
//                 {
//                     std::cerr << "POSITION: Corresponding fiber_id not found: " << fiber_id
//                               << " for SiPmID: " << c.first << " with SiPmID fold: " << sipm_fold
//                               << std::endl;
//                     continue;
//                 }
//
//                 std::map<int, double>::iterator iter_e;
//
//                 if ((iter_e = fPxEnScin->find(fiber_id)) != fPxEnScin->end())
//                 {
//                     tree.energy_dep = iter_e->second;
//                 }
//                 else
//                 {
//                     std::cerr << "ENERGY: Corresponding fiber_id not found: " << fiber_id
//                               << " for SiPmID: " << c.first << " with SiPmID fold: " << sipm_fold
//                               << std::endl;
//                     continue;
//                 }
//
//                 tree.address = sipm_map[fiber_id];
//                 if (c.first >= sipm_fold) tree.address.s = 'r';
//                 tree.data.counts = c.second;
//                 tree.data.time = times[c.first];
//
//                 if (unp) unp->execute(0, 0, subevent, vSiPMData, 1);
//                 tree.kine.clear();
//             }
//         }
    }
    return true;
}

void SSimSource::print()
{
    std::cout << "Size of fMCPosition_e: " << fMCPosition_e->size() << std::endl;
    std::cout << "Size of fMCInteractions_e: " << fMCInteractions_e->size() << std::endl;
    std::cout << "Size of fMCPosition_p: " << fMCPosition_p->size() << std::endl;
    std::cout << "Size of fMCInteractions_p: " << fMCInteractions_p->size() << std::endl;
    std::cout << "Size of fMCEnergyDeps_e: " << fMCEnergyDeps_e->size() << std::endl;
    std::cout << "Size of fMCEnergyDeps_p: " << fMCEnergyDeps_p->size() << std::endl;
    std::cout << "Size of vFibreData: " << vFibreData->size() << std::endl;
    std::cout << "Size of vSiPMData: " << vSiPMData->size() << std::endl;
}
