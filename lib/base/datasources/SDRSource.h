// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SDRSOURCE_H
#define SDRSOURCE_H

#include "SiFiConfig.h"
#include "sifi_export.h"

#include "SFibersStackCalSim.h"
#include "SGeantTrack.h"
#include "SRootSource.h"

#include <ComptonCameraHitClass.hh>

#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>
#include <TVector3.h>

#include <cstddef>
#include <fstream>
#include <string>

class DRSiFiCCSetup;
class DRSiPMModel;
class TChain;

struct TREE_Events
{
    TClonesArray* fHitArray;
};

struct TREE_Kine
{
    TVector3 pos, dir;
    double E;
    SGeantTrack::Type type;
};

struct TREE_Address
{
    int m;
    int l;
    int f;
    char s;
};

struct TREE_hit
{
    int counts;
    float time;
};

struct TREE_simdata
{
    float x, y, z;
};

struct TREE_all
{
    TREE_Address address;
    TREE_Events events;
    TREE_hit data;
    TVector3 pos;
    Float_t energy_dep;
    SFibersStackCalSim::InteractionType type;
    std::vector<TREE_Kine> kine;
};

/**
 * Extends SDataSOurce to read data from Desktop Digitizer.
 */
class SIFI_EXPORT SDRSource : public SRootSource
{
public:
    explicit SDRSource(/*uint16_t subevent*/);

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    virtual void addInput(const std::string& filename) override;

protected:
    TChain* chain2;
    TChain* chain3;

private:
    uint16_t subevent; ///< subevent id

    // for "Events" in chain
    std::vector<double>* fPrimEnergy{nullptr};
    std::vector<TVector3>* fSourcePosition{nullptr};  //!< BranchAddress RealData
    std::vector<TVector3>* fSourceDirection{nullptr}; //!< BranchAddress RealData

    double fScaEnergyElectron{0}; //!< energy of the recoil electron
    double fScaEnergyPhoton{0};   //!< energy of the scattered photon
    double fComptonTime{0};       //!< time when the compton scattering occured

    TVector3* fComptonPosition{nullptr}; //!< position where the compton scattering occured
    // TVector3* fPhotonPosition{nullptr};                 //!< first position where the scattered
    // photon is absorbed TVector3* fElectronPosition{nullptr};               //!< position where
    // the recoilelectron is absorbed

    TVector3* fPhotonDirection{nullptr}; //!< direction of the scattered photon

    std::vector<TVector3>* fPhotonPositions{
        nullptr}; //!< positions where the scattered photon and its secondaries interacted
    std::vector<TVector3>* fElectronPositions{
        nullptr}; //!< position where the recoilelectron and secondaries interacted

    std::vector<int>*
        fPhotonInteractions; //!< the interaction and secondary generations of the positions
    std::vector<int>* fElectronInteractions; //!< interactions to the recoilelectrons and

    // helper
    std::map<int, TREE_Address> sipm_map;
    int sipm_fold{0}; // index at which counting for right side starts

    // for "DetectorEvent" in chain2
    std::map<int, TVector3>* fPxPosPhot{nullptr};
    std::map<int, TVector3>* fPxPosElec{nullptr};
    std::map<int, TVector3>* fPxPosScin{nullptr};
    std::map<int, double>* fPxEnPhot{nullptr};
    std::map<int, double>* fPxEnElec{nullptr};
    std::map<int, double>* fPxEnScin{nullptr};

    TREE_all tree;
    DRSiFiCCSetup* ccsetup{nullptr};
    DRSiPMModel* pmmodel{nullptr};
};

#endif /* SDRSOURCE_H */
