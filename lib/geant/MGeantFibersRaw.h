// @(#)lib/geant:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $MAPTSYS/LICENSE.                         *
 * For the list of contributors see $MAPTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef MGEANTFIBERSRAW_H
#define MGEANTFIBERSRAW_H

#include "TObject.h"

class MGeantFibersRaw : public TObject
{
protected:
    // members
    Int_t module;                           ///< module
    Int_t fiber;                            ///< fiber

    Double_t energyLoss;                    ///< energy loss
    Double_t energyDeposition;              ///< energy deposition
    Double_t energyDepositionQuenching;     ///< quenching
    Double_t totalEnergy;                   ///< total energy
    Double_t kineticEnergy;                 ///< kinetic energy
    Int_t opticalPhotonCount;               ///< number of photons
    Int_t fiberHits;                        ///< number of fibers hits

public:
    // constrructor
    MGeantFibersRaw();
    // destructor
    virtual ~MGeantFibersRaw() {}

    // inherited from ROOT
    void Clear(Option_t* opt = "");

    // methods
    void clear();

    /// Set address
    /// \param m module
    /// \param f fiber
    void setAddress(Int_t m, Int_t f) { module = m; fiber = f; }
    /// Get address
    /// \param m module
    /// \param f fiber
    void getAddress(Int_t & m, Int_t & f) const { m = module; f = fiber; }

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

    void print() const;

private:
    // Needed for creation of shared library
    ClassDef(MGeantFibersRaw, 1);
};

#endif /* MGEANTFIBERSRAW_H */
