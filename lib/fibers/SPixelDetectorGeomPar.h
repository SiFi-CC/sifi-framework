// @(#)lib/fibers:$Id$

#ifndef SPIXELDETECTORGEOMPAR_H
#define SPIXELDETECTORGEOMPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <RtypesCore.h> // for Int_t, Float_t
#include <TArrayF.h>
#include <TArrayI.h>
#include <TVector3.h>

class SParContainer;

class SIFI_EXPORT SPixelDetectorGeomPar : public SPar
{
protected:
    // members
    struct SingleModule ///< single module configuration
    {
        TVector3 modulePosition; ///< position of the module (Aachen coordinate system)
        TVector3 boardPosition; ///< position of the SiPM board
        Int_t numSiPMLayers{0}; ///< number of SiPM layers
        Int_t numNeedleLayers{0}; ///< number of needle layers
        TArrayI numSiPMsPerLayer; ///< number of SiPMs in each layer
        TArrayI numNeedlesPerLayer; ///< number of needles in each layer
    };

    SingleModule* mod; ///< params for the single module

public:
    SPixelDetectorGeomPar();
    virtual ~SPixelDetectorGeomPar();

    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    TVector3 getModulePosition() const;
    TVector3 getBoardPosition() const;
    Int_t getNumSiPMLayers() const;
    Int_t getNumNeedleLayers() const;
    Int_t getNumSiPMsPerLayer(Int_t layer) const;
    Int_t getNumNeedlesPerLayer(Int_t layer) const;
    void getswSiPMIDFromAddress(Int_t& id, Int_t layer, Int_t element, char side) const;
    void getswNeedleIDFromAddress(Int_t& id, Int_t layer, Int_t needle) const;
    bool getAddressFromswSiPMID(Int_t id, Int_t& layer, Int_t& element, char& side) const;
    bool getAddressFromswNeedleID(Int_t id, Int_t& layer, Int_t& needle) const;

    TVector3 getModulePosition(Int_t module) const;
    TVector3 getBoardPosition(Int_t module) const;
    Int_t getNumSiPMLayers(Int_t module) const;
    Int_t getNumNeedleLayers(Int_t module) const;
    Int_t getNumSiPMsPerLayer(Int_t module, Int_t layer) const;
    Int_t getNumNeedlesPerLayer(Int_t module, Int_t layer) const;
    void getswSiPMIDFromAddress(Int_t& id, Int_t module, Int_t layer, Int_t element, char side) const;
    void getswNeedleIDFromAddress(Int_t& id, Int_t module, Int_t layer, Int_t needle) const;
    bool getAddressFromswSiPMID(Int_t id, Int_t& module, Int_t& layer, Int_t& element, char& side) const;
    bool getAddressFromswNeedleID(Int_t id, Int_t& module, Int_t& layer, Int_t& needle) const;
};

#endif /* SPIXELDETECTORGEOMPAR_H */
