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
    Float_t fGeantEloss{0};     ///< energy loss from Geant
    Float_t fGeantX{0};         ///< geant X
    Float_t fGeantY{0};         ///< geant Y
    Float_t fGeantZ{0};         ///< geant Z
    enum InteractionType { PHOTON, ELECTRON, SCINT } type;

public:
    // constructors
    SFibersStackCalSim() = default;
    // detructor
    virtual ~SFibersStackCalSim() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "") override;

    /// Set energy loss simulated in Geant
    /// \param e energy loss in Geant
    void setGeantEnergyLoss(Float_t e) { fGeantEloss = e; }
    /// Get energy loss simulated in Geant
    /// \return energy loss in Geant
    Float_t getGeantEnergyLoss() const { return fGeantEloss; }

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

    void print() const override;

private:
    ClassDefOverride(SFibersStackCalSim, 1);
};

#endif /* SFIBERSSTACKCALSIM_H */
