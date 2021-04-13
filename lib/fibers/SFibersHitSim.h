// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSHITSIM_H
#define SFIBERSHITSIM_H

#include "sifi_export.h" // for SIFI_EXPORT

#include "SFibersHit.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDefOverride
#include <RtypesCore.h> // for Float_t, Option_t
#include <TVector3.h>   // for TVector3

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersHitSim : public SFibersHit
{
public:
    // data members
    Float_t fGeantEloss{0};        ///< energy loss from Geant
    TVector3 fGeantPoint{0, 0, 0}; ///< geant X,Y,Z
    enum InteractionType           ///< describes interaction type
    {
        PHOTON,   ///< photon
        ELECTRON, ///< electron
        SCINT     ///< scintilation
    } type;       ///< Interaction type

public:
    // constructors
    SFibersHitSim() = default;
    // detructor
    virtual ~SFibersHitSim() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "") override;

    /// Set energy loss simulated in Geant
    /// \param e energy loss in Geant
    void setGeantEnergyLoss(Float_t e) { fGeantEloss = e; }
    /// Get energy loss simulated in Geant
    /// \return energy loss in Geant
    Float_t getGeantEnergyLoss() const { return fGeantEloss; }

    /// Set Geant X coordinate
    /// \param p coordinate value
    void setGeantPoint(TVector3 p) { fGeantPoint = p; }
    /// Get Geant X coordinate
    /// \return X value
    TVector3& getGeantPoint() { return fGeantPoint; }
    /// Get Geant XYZ coordinate
    /// \return XYZ value
    const TVector3& getGeantPoint() const { return fGeantPoint; }

    virtual void print() const override;

private:
    ClassDefOverride(SFibersHitSim, 1);
};

#endif /* SFIBERSHITSIM_H */
