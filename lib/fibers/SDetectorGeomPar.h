// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/ //TODO keep this? modify?

#ifndef SDETECTORGEOMPAR_H
#define SDETECTORGEOMPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <RtypesCore.h> // for Int_t, Float_t
#include <TArrayF.h>
#include <TArrayI.h>
#include <TVector3.h>

class SParContainer;

class SIFI_EXPORT SDetectorGeomPar : public SPar
{
protected:
    // members
    Int_t modules;      ///< number of modules
    struct SingleModule ///< single module configuration
    {
        TVector3 modulePosition; ///< position of the module (Aachen coordinate system)
        TVector3 bottomBoardPosition; ///< position of the bottom board
        TVector3 topBoardPosition; ///< position of the top board
        Float_t bottomBoardZRotationAngle; ///< rotation angle around the Z axis for the bottom board
        Float_t topBoardZRotationAngle; ///< rotation angle around the Z axis for the top board
        Int_t numSiPMLayers; ///< number of SiPM layers for the bottom board
        Int_t numFiberLayers; ///< number of fiber layers
        TArrayI numSiPMsPerLayer; ///< number of SiPMs in each layer for the bottom board
        TArrayI numFibersPerLayer; ///< number of fibers in each layer
    };

    SingleModule* mods; ///< params for each module

    std::map<Int_t, TVector3> localPositionSiPM; ///< map with the swID and local position of each SiPM
    std::map<Int_t, TVector3> localPositionFiber; ///< map with the swID and local position of each fiber

public:
    SDetectorGeomPar();
    virtual ~SDetectorGeomPar();

    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Get number of modules
    /// \return number of modules
    Int_t getModules() const { return modules; }
    TVector3 getModulePosition(Int_t m) const;
    TVector3 getBottomBoardPosition(Int_t m) const;
    TVector3 getTopBoardPosition(Int_t m) const;
    Float_t getBottomBoardZRotationAngle(Int_t m) const;
    Float_t getTopBoardZRotationAngle(Int_t m) const;
    Int_t getNumSiPMLayers(Int_t m) const;
    Int_t getNumFiberLayers(Int_t m) const;
    Int_t getNumSiPMsPerLayer(Int_t m, Int_t l) const;
    Int_t getNumFibersPerLayer(Int_t m, Int_t l) const;
    void getswSiPMIDFromAddress(Int_t& id, Int_t m, Int_t l, Int_t f, char s) const;
    void getswFiberIDFromAddress(Int_t& id, Int_t m, Int_t l, Int_t f) const;
    void getAddressFromswSiPMID(Int_t id, Int_t& m, Int_t& l, Int_t& f, char& s) const;
    void getAddressFromswFiberID(Int_t id, Int_t& m, Int_t& l, Int_t& f) const;
    TVector3 getLocalSiPMPosition (Int_t id) const;
    TVector3 getLocalFiberPosition(Int_t id) const;
    TVector3 getGlobalSiPMPosition(Int_t id) const;
    TVector3 getGlobalSiPMPosition(Int_t m, Int_t l, Int_t f, char s) const;
    TVector3 getGlobalFiberPosition(Int_t id) const;
    TVector3 getGlobalFiberPosition(Int_t m, Int_t l, Int_t f) const;
};

#endif /* SDETECTORGEOMPAR_H */
