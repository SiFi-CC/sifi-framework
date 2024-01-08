#ifndef SSIMSOURCE_H
#define SSIMSOURCE_H

#include "sifi_export.h"
#include "SRootSource.h"

#include <TVector3.h>
#include "FibreData.hh"
#include "SiPMData.hh"
#include "SDetectorGeomPar.h"

class SIFI_EXPORT SSimSource : public SRootSource
{
public:
    explicit SSimSource();

    virtual bool open() override;
    virtual bool close() override;
    virtual bool readCurrentEvent() override;
    void print();

protected:

private:
    uint16_t subevent; ///< subevent id

    Int_t fEventNumber; ///< number of event
    Int_t fMCSimulatedEventType; ///< MC encoding
    Double_t fMCEnergyPrimary; ///< energy of the first primary photon
    Double_t fMCEnergy_e; ///< energy of the Compton electron
    Double_t fMCEnergy_p; ///< energy of the scattered primary photon
    TVector3* fMCPosition_source; ///< position where the primary photon was emitted in case of Compton scattering in the scatterer, otherwise (0,0,0)
    TVector3* fMCDirection_source; ///< momentum of the primary photon in case of Compton scattering in the scatterer, otherwise (0,0,0)
    TVector3* fMCComptonPosition; ///< position where the Compton scattering occurred
    TVector3* fMCDirection_scatter; ///< momentum of the primary photon after Compton scattering
    std::vector<TVector3>* fMCPosition_e; ///< all interaction positions of the Compton electrons and its secondaries
    std::vector<Int_t>* fMCInteractions_e; ///< interactions of the Compton electrons and its secondaries, MC encoding
    std::vector<TVector3>* fMCPosition_p; ///< all interaction positions of the photon and its secondaries besides the Compton electron
    std::vector<Int_t>* fMCInteractions_p; ///< interactions of the photon and its secondaries besides the Compton electron, MC encoding
    std::vector<Double_t>* fMCEnergyDeps_e; ///< energy deposit of the Compton electron and all its secondaries
    std::vector<Double_t>* fMCEnergyDeps_p; ///< energy deposits of all primary photons and all their secondaries
    std::vector<FibreData>* vFibreData; ///< MC information about fiber hits (ID, position, energy, time)
    std::vector<SiPMData>* vSiPMData; ///< information about SiPM hits (ID, position, photon count, timestamp)
};

#endif /* SSIMSOURCE_H */
