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

#include "sifi_export.h"

#include "SFibersCalSim.h"
#include "SGeantTrack.h"
#include "SRootSource.h"

#include <RtypesCore.h> // for Float_t
#include <TVector3.h>

#include <cstdint> // for uint16_t
#include <map>     // for map
#include <string>
#include <vector> // for vector

class TClonesArray;
class DRSiFiCCSetup;
class DRSiPMModel;
class TChain;

/// Encapsulates array of hits in a SiPm
struct TREE_Events
{
    TClonesArray* fHitArray; ///< SiPm hits data
};

/// Encapsulates kine (sim) data
struct TREE_Kine
{
    ///@{
    TVector3 pos, dir; ///< interaction point and direction
    ///@}
    double E;               ///< energy
    SGeantTrack::Type type; ///< type of track
};

/// Encapsulates fired fiber address
struct TREE_Address
{
    int m;  ///< module
    int l;  ///< layer
    int f;  ///< fiber
    char s; ///< side
};

/// Encapsulates hit statistics data
struct TREE_hit
{
    int counts; ///< number of hits
    float time; ///< time of the signal
};

/// Encapsulates sim data position FIXME what is different to kine? Can we merge?
struct TREE_simdata
{
    ///@{
    float x, y, z; ///< hit position
    ///@}
};

/// Encapsulates all other TREE_ structures
struct TREE_all
{
    TREE_Address address;                ///< address of the event
    TREE_Events events;                  ///< SiPm hits
    TREE_hit data;                       ///< data
    TVector3 pos;                        ///< position
    Float_t energy_dep;                  ///< energy deposition
    SFibersCalSim::InteractionType type; ///< interaction type
    std::vector<TREE_Kine> kine;         ///< collection of kine data
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
    TChain* chain2; ///< chain for some other data
    TChain* chain3; ///< chain for some other data

private:
    uint16_t subevent; ///< subevent id

    // for "Events" in chain
    std::vector<double>* fPrimEnergy{nullptr};        ///< primary energy
    std::vector<TVector3>* fSourcePosition{nullptr};  ///< BranchAddress RealData
    std::vector<TVector3>* fSourceDirection{nullptr}; ///< BranchAddress RealData

    double fScaEnergyElectron{0}; ///< energy of the recoil electron
    double fScaEnergyPhoton{0};   ///< energy of the scattered photon
    double fComptonTime{0};       ///< time when the compton scattering occured

    TVector3* fComptonPosition{nullptr}; //!< position where the compton scattering occured
    // TVector3* fPhotonPosition{nullptr};                 //!< first position where the scattered
    // photon is absorbed TVector3* fElectronPosition{nullptr};               //!< position where
    // the recoilelectron is absorbed

    TVector3* fPhotonDirection{nullptr}; //!< direction of the scattered photon

    std::vector<TVector3>* fPhotonPositions{
        nullptr}; ///< positions where the scattered photon and its secondaries interacted
    std::vector<TVector3>* fElectronPositions{
        nullptr}; ///< position where the recoilelectron and secondaries interacted

    std::vector<int>*
        fPhotonInteractions; ///< the interaction and secondary generations of the positions
    std::vector<int>* fElectronInteractions; ///< interactions to the recoilelectrons and

    // helper
    std::map<int, TREE_Address> sipm_map; ///< SiPm map
    int sipm_fold{0};                     ///< index at which counting for right side starts

    // for "DetectorEvent" in chain2
    std::map<int, TVector3>* fPxPosScin{nullptr}; ///< scintillation position
    std::map<int, double>* fPxEnScin{nullptr};    ///< scintillation energy

    TREE_all tree;                   ///< tree to store all data for unpacker
    DRSiFiCCSetup* ccsetup{nullptr}; ///< compton camera setup
    DRSiPMModel* pmmodel{nullptr};   ///< PM model
};

#endif /* SDRSOURCE_H */
