// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSCALSIM_H
#define SFIBERSCALSIM_H

#include "SFibersCal.h"

#include "sifi_export.h" // for SIFI_EXPORT

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDefOverride
#include <RtypesCore.h> // for Float_t, Option_t
#include <TVector3.h>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersCalSim : public SFibersCal
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
    SFibersCalSim() = default;
    // detructor
    virtual ~SFibersCalSim() = default;

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
    /// \copydoc getGeantPoint()
    const TVector3& getGeantPoint() const { return fGeantPoint; }

    void print() const override;

private:
    ClassDefOverride(SFibersCalSim, 1);
};

#endif /* SFIBERSCALSIM_H */
