// @(#)lib/geant:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SGEANTFIBERSRAW_H
#define SGEANTFIBERSRAW_H

#include "sifi_export.h"

#include <TObject.h>

class SIFI_EXPORT SGeantFibersRaw : public TObject
{
  protected:
    // members
    Int_t module; ///< module
    Int_t fiber;  ///< fiber

    Double_t energyLoss;                ///< energy loss
    Double_t energyDeposition;          ///< energy deposition
    Double_t energyDepositionQuenching; ///< quenching
    Double_t totalEnergy;               ///< total energy
    Double_t kineticEnergy;             ///< kinetic energy
    Int_t opticalPhotonCount;           ///< number of photons
    Int_t fiberHits;                    ///< number of fibers hits
    Float_t light_l;                    ///< light left
    Float_t light_r;                    ///< light right
    Float_t time_l;                     ///< time left
    Float_t time_r;                     ///< time right

  public:
    // constrructor
    SGeantFibersRaw();
    // destructor
    virtual ~SGeantFibersRaw() = default;

    // inherited from ROOT
    void Clear(Option_t* opt = "");

    // methods
    void clear();

    /// Set address
    /// \param m module
    /// \param f fiber
    void setAddress(Int_t m, Int_t f)
    {
        module = m;
        fiber = f;
    }
    /// Get address
    /// \param m module
    /// \param f fiber
    void getAddress(Int_t& m, Int_t& f) const
    {
        m = module;
        f = fiber;
    }

    /// Set energy loss
    /// \param e energy loss
    void setEnergyLoss(Double_t e) { energyLoss = e; }
    /// Add energy loss
    /// \param e energy loss
    void addEnergyLoss(Double_t e) { energyLoss += e; }
    /// Get energy loss
    /// \return energy loss
    Double_t getEnergyLoss() const { return energyLoss; }

    /// Set energy deposition
    /// \param e energy deposition
    void setEnergyDeposition(Double_t e) { energyDeposition = e; }
    /// Add energy deposition
    /// \param e energy deposition
    void addEnergyDeposition(Double_t e) { energyDeposition += e; }
    /// Get energy deposition
    /// \return energy deposition
    Double_t getEnergyDeposition() const { return energyDeposition; }

    /// Set energy quenching
    /// \param e energy quenching
    void setEnergyQuenching(Double_t e) { energyDepositionQuenching = e; }

    /// Set photons
    /// \param p photons number
    void setPhotons(Int_t p) { opticalPhotonCount += p; }
    /// Set total energy
    /// \param e total energy
    void setTotalEnergy(Double_t e) { totalEnergy += e; }
    /// Get total energy
    /// \return total energy
    Double_t getTotalEnergy() const { return totalEnergy; }
    /// Set kinetic energy
    /// \param e kinetic energy
    void setKineticEnergy(Double_t e) { kineticEnergy += e; }
    /// Get kinetic energy
    /// \return kinetic energy
    Double_t getKineticEnergy() const { return kineticEnergy; }

    /// Set light on the left side
    /// \param l kinetic energy
    void setLightL(Float_t l) { light_l = l; }
    /// Get light on the left side
    /// \return light
    Double_t getLightL() const { return light_l; }
    /// Set light on the left side
    /// \param l kinetic energy
    void setLightR(Float_t l) { light_r = l; }
    /// Get light on the right side
    /// \return light
    Double_t getLightR() const { return light_r; }

    /// Set time on the left side
    /// \param t kinetic energy
    void setTimeL(Float_t t) { time_l = t; }
    /// Get time on the left side
    /// \return time
    Double_t getTimeL() const { return time_l; }
    /// Set time on the left side
    /// \param t kinetic energy
    void setTimeR(Float_t t) { time_r = t; }
    /// Get time on the right side
    /// \return time
    Double_t getTimeR() const { return time_r; }

    void print() const;

  private:
    // Needed for creation of shared library
    ClassDef(SGeantFibersRaw, 1);
};

#endif /* SGEANTFIBERSRAW_H */
