// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCALSIM_H
#define SFIBERSSTACKCALSIM_H

#include "SFibersStackCal.h"

class SIFI_EXPORT SFibersStackCalSim : public SFibersStackCal
{
public:
    // data members
    Float_t T;                  ///< kinetic energy
    Float_t E;                  ///< total energy
    Float_t dEdx;               ///< energy loss
    Float_t fGeantX;            ///< geant X
    Float_t fGeantY;            ///< geant Y
    Float_t fGeantZ;            ///< geant Z
    enum InteractionType { PHOTON, ELECTRON, SCINT } type;

public:
    // constructors
    SFibersStackCalSim();
    // detructor
    virtual ~SFibersStackCalSim() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "") override;

    // methods
    /// Set kinetic energy
    /// \param t kinetic energy
    void setKineticEnergy(Float_t t) { T = t; }
    /// Get kinetic energy
    /// \return kinetic energy
    Float_t getKineticEnergy() const { return T; }

    /// Set total energy
    /// \param e total energy
    void setTotalEnergy(Float_t e) { E = e; }
    /// Get total energy
    /// \return total energy
    Float_t getTotalEnergy() const { return E; }

    /// Set energy loss
    /// \param e energy loss
    void setEnergyLoss(Float_t e) { dEdx = e; }
    /// Get energy loss
    /// \return energy loss
    Float_t getEnergyLoss() const { return dEdx; }

    /// Set Geant X coordinate
    /// \param v coordinate value
    void setGeantX(Float_t v) { fGeantX = v; }
    /// Get Geant X coordinate
    /// \return X value
    Float_t getGeantX() const { return fGeantX; }

    /// Set Geant Y coordinate
    /// \param v coordinate value
    void setGeantY(Float_t v) { fGeantY = v; }
    /// Get Geant Y coordinate
    /// \return Y value
    Float_t getGeantY() const { return fGeantY; }

    /// Set Geant Z coordinate
    /// \param v coordinate value
    void setGeantZ(Float_t v) { fGeantZ = v; }
    /// Get Geant Z coordinate
    /// \return Z value
    Float_t getGeantZ() const { return fGeantZ; }

    void print() const;

private:
    ClassDefOverride(SFibersStackCalSim, 1);
};

#endif /* SFIBERSSTACKCALSIM_H */
