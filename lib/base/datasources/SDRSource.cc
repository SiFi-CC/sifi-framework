// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SDRSource.h"
#include "SFibersStackCalSim.h"
#include "SSiFiCCDetResImporter.h"
#include "SUnpacker.h"

#include <DR_CCSetup.hh>

#include <TChain.h>

#include <iostream>
#include <map>

/**
 * Constructor. Requires subevent id for unpacked source.
 *
 * \param subevent subevent id
 */
SDRSource::SDRSource() : SRootSource("Events"), subevent(0)
{
    chain2 = new TChain("DetectorEvents");
    chain3 = new TChain("Setup_stats");

    tree.address.m = 0;
    tree.address.l = 0;
    tree.address.f = 0;
    tree.address.s = 'l';

    // "Events" tree
    chain->SetBranchAddress("PrimEnergy", &fPrimEnergy);
    chain->SetBranchAddress("SourcePos", &fSourcePosition);
    chain->SetBranchAddress("SDir", &fSourceDirection);

    chain->SetBranchAddress("ScaEnergy_e", &fScaEnergyElectron);
    chain->SetBranchAddress("ScaEnergy_p", &fScaEnergyPhoton);
    chain->SetBranchAddress("ComptonTime", &fComptonTime);

    chain->SetBranchAddress("CompPos", &fComptonPosition);

    chain->SetBranchAddress("PDir", &fPhotonDirection);

    chain->SetBranchAddress("PPos", &fPhotonPositions);
    chain->SetBranchAddress("EPos", &fElectronPositions);

    chain->SetBranchAddress("EInteractions", &fElectronInteractions);
    chain->SetBranchAddress("PInteractions", &fPhotonInteractions);

    // "DetectorEvent" tree
    chain2->SetBranchAddress("Hitsarray", &tree.events.fHitArray);
    chain2->SetBranchAddress("Pixel_Position_Photon", &fPxPosPhot);
    chain2->SetBranchAddress("Pixel_Position_Electron", &fPxPosElec);
    chain2->SetBranchAddress("Pixel_Position_Scin", &fPxPosScin);

    pmmodel = new DRSiPMModel(0.4, 0.06, 3e6, 500, 10, false);
}

/**
 * Open source and init respective unpacker.
 *
 * \return success
 */
bool SDRSource::open()
{
    if (unpackers.size() == 0) return false;

    chain->GetEntry(0);
    chain2->GetEntry(0);
    chain3->GetEntry(0);

    // create fibers map
    ccsetup = new DRSiFiCCSetup(chain3, false);
    const size_t modules = ccsetup->GetNumberOfDetectors();
    size_t acc_layers = 0;
    size_t id_offset = 0;
    for (int m = 0; m < modules; ++m)
    {
        const size_t layers = ccsetup->GetLayerAmount(m);
        const size_t fibers = ccsetup->GetVoxelAmount(m);

        for (int l = 0; l < layers; ++l)
        {
            for (int f = 0; f < fibers; ++f)
            {
                size_t base_l = id_offset + (l * fibers + f);
                size_t base_r = id_offset + layers * fibers + (l * fibers + f);

                sipm_map[base_l] = {m, l, f, 'l'};
                sipm_map[base_r] = {m, l, f, 'r'};
                fiber_map[base_l] = acc_layers * fibers + l * fibers + f;
                fiber_map[base_r] = acc_layers * fibers + l * fibers + f;
            }
        }

        acc_layers += layers;
        id_offset += layers * fibers * 2;
    }
    ccsetup->GetDetectorPosition(0).Print();
    ccsetup->GetDetectorPosition(1).Print();

    // Uncomment to verify ID vs fiber adress

    //     for (auto & addr : sipm_map)
    //         printf("ID=%d  M=%d  L=%d  F=%d  SIDE=%c\n", addr.first,
    //         addr.second.m, addr.second.l, addr.second.f, addr.second.s);

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

    printf("Files in chain = %d\n", chain->GetFileNumber());

    return true;
}

bool SDRSource::close()
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

bool SDRSource::readCurrentEvent()
{
    if (unpackers.size() == 0) return false;

    long ce = getCurrentEvent();
    chain->GetEntry(ce);
    chain2->GetEntry(ce);

    pmmodel->DigitizeEvent(ce, tree.events.fHitArray);
    auto counts = pmmodel->GetSiPMCounts();
    auto times = pmmodel->GetSiPMTimes();

    tree.kine.clear();

    if (fPrimEnergy->size())
    {
        TREE_Kine k = {(*fSourcePosition)[0], (*fSourceDirection)[0], (*fPrimEnergy)[0],
                       SGeantTrack::G_PRIM};
        tree.kine.push_back(k);
    }
    if (fComptonPosition && fPhotonDirection)
    {
        TREE_Kine k = {*fComptonPosition, *fPhotonDirection, 100, SGeantTrack::G_COMP};
        tree.kine.push_back(k);
    }
    if (fPhotonPositions)
    {
        int n = fPhotonPositions->size();
        if (n)
        {
            TREE_Kine k = {*fComptonPosition, (*fPhotonPositions)[0] - *fComptonPosition, 100,
                           SGeantTrack::G_COMP};
            tree.kine.push_back(k);
        }
        for (int i = 0; i < n - 1; ++i)
        {
            TREE_Kine k = {(*fPhotonPositions)[i],
                           (*fPhotonPositions)[i + 1] - (*fPhotonPositions)[i], 100,
                           SGeantTrack::G_COMP};
            tree.kine.push_back(k);
        }
        if (n)
        {
            TREE_Kine k = {(*fPhotonPositions)[n - 1], TVector3(0, 0, 1), 100,
                           SGeantTrack::G_COMP_ABS};
            tree.kine.push_back(k);
        }
    }
    if (fElectronPositions)
    {
        int n = fElectronPositions->size();
        if (fComptonPosition && n)
        {
            TREE_Kine k = {*fComptonPosition, (*fElectronPositions)[0] - *fComptonPosition, 100,
                           SGeantTrack::E_COMP};
            tree.kine.push_back(k);
        }
        for (int i = 0; i < n - 1; ++i)
        {
            TREE_Kine k = {(*fElectronPositions)[i],
                           (*fElectronPositions)[i + 1] - (*fElectronPositions)[i], 100,
                           SGeantTrack::E_COMP};
            tree.kine.push_back(k);
        }
        if (n)
        {
            TREE_Kine k = {(*fElectronPositions)[n - 1], TVector3(0, 0, 1), 100,
                           SGeantTrack::E_COMP_ABS};
            tree.kine.push_back(k);
        }
    }

    if (subevent != 0x0000)
    {
        if (!unpackers[subevent]) abort();
        // TODO must pass event number to the execute
        SSiFiCCDetResImporter* unp = dynamic_cast<SSiFiCCDetResImporter*>(unpackers[subevent]);
        for (auto& c : counts)
        {

            tree.address = sipm_map[c.first];
            tree.data.counts = c.second;
            tree.data.time = times[c.first];
            // int fiber_id = fiber_map[c.first];

            if (unp) unp->execute(0, 0, subevent, &tree, 1);
            tree.kine.clear();
        }
    }
    else
    {
        for (const auto& u : unpackers)
        {
            SSiFiCCDetResImporter* unp = dynamic_cast<SSiFiCCDetResImporter*>(u.second);
            for (auto& c : counts)
            {
                tree.address = sipm_map[c.first];
                tree.data.counts = c.second;
                tree.data.time = times[c.first];
                int fiber_id = fiber_map[c.first];
                std::map<int, TVector3>::iterator iter;

                if ((iter = fPxPosPhot->find(fiber_id)) != fPxPosPhot->end())
                {
                    tree.pos = iter->second;
                    tree.type = SFibersStackCalSim::InteractionType::PHOTON;
                }
                else if ((iter = fPxPosElec->find(fiber_id)) != fPxPosElec->end())
                {
                    tree.pos = iter->second;
                    tree.type = SFibersStackCalSim::InteractionType::ELECTRON;
                }
                else if ((iter = fPxPosScin->find(fiber_id)) != fPxPosScin->end())
                {
                    tree.pos = iter->second;
                    tree.type = SFibersStackCalSim::InteractionType::SCINT;
                }
                else
                {
                    std::cerr << "does not work\n";
                    continue;
                }

                if (unp) unp->execute(0, 0, u.first, &tree, 1);
                tree.kine.clear();
            }
        }
    }

    return true;
}

void SDRSource::addInput(const std::string& filename)
{
    SRootSource::addInput(filename);
    chain2->Add(filename.c_str());
    chain3->Add(filename.c_str());
}
