// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SSIPMSGEOMPAR_H
#define SSIPMSGEOMPAR_H

#include "sifi_export.h"

#include "SPar.h"

#include <RtypesCore.h> // for Int_t, Float_t
#include <TArrayF.h>
#include <TArrayI.h>

class SParContainer;

class SIFI_EXPORT SSiPMsGeomPar : public SPar
{
protected:
    // members
    Int_t modules;      ///< number of modules
    struct SingleModule ///< single module configuration
    {
        Float_t module_z;       ///< z-coordinate of the module reference point (center)
        Int_t layers;           ///< numbre of layers
        TArrayI sipms;         ///< number of SiPMs
        TArrayF sipm_offset_x; ///< offset of the first fiber in the layer
        TArrayF sipm_offset_y; ///< offset of the layers
        TArrayF sipm_offset_z; ///< offset of the layers
        TArrayF sipms_pitch;   ///< SiPM pitch in a layer
        TArrayF sipms_array_pitch;   ///< pitch between SiPM Broadcom arrays (each 4 SiPMs)
        TArrayF sipms_x; ///< SiPM X positions in a layer
        Float_t sipms_y_bottom; ///< SiPM Y position on bottom side
        Float_t sipms_y_top; ///< SiPM Y position on top side
        
    };

    SingleModule* mods; ///< params for each module

public:
    SSiPMsGeomPar();
    virtual ~SSiPMsGeomPar();

    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Get number of modules
    /// \return number of modules
    Int_t getModules() const { return modules; }
    Float_t getModuleZ(Int_t m) const;
    Int_t getLayers(Int_t m) const;
    Int_t getSiPMs(Int_t m, Int_t l) const;
    Float_t getSiPMOffsetX(Int_t m, Int_t l) const;
    Float_t getSiPMOffsetY(Int_t m, Int_t l) const;
    Float_t getSiPMOffsetZ(Int_t m, Int_t l) const;
    Float_t getSiPMsPitch(Int_t m, Int_t l) const;
    Float_t getSiPMsArrayPitch(Int_t m, Int_t l) const;
    Float_t getSiPMsX(Int_t m, Int_t l) const;
    Float_t getSiPMsYBottom(Int_t m, Int_t l) const;
    Float_t getSiPMsYTop(Int_t m, Int_t l) const;

};

#endif /* SSIPMSGEOMPAR_H */
