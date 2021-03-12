// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSGEOMPAR_H
#define SFIBERSGEOMPAR_H

#include "sifi_export.h"

#include <TArrayF.h>
#include <TArrayI.h>

#include "SPar.h"

class SIFI_EXPORT SFibersGeomPar : public SPar
{
protected:
    // members
    Int_t type;         ///< geometry type, 0 - aligned, 1 - crossed
    Int_t modules;      ///< number of modules
    struct SingleModule ///< single module configuration
    {
        Float_t module_z;       ///< z-coordinate of the module reference point (center)
        Int_t layers;           ///< numbre of layers
        TArrayI fibers;         ///< number of fibers
        TArrayF layer_rotation; ///< layer rotation around the axis
        TArrayF fiber_offset_x; ///< offset of the first fiber in the layer
        TArrayF fiber_offset_y; ///< offset of the layers
        TArrayF fibers_pitch;   ///< fibers pitch in a layer
    };

    SingleModule* mods; ///< params for each module

public:
    SFibersGeomPar();
    virtual ~SFibersGeomPar();

    bool getParams(SParContainer* parcont) override;
    bool putParams(SParContainer* parcont) const override;
    void clear() override;
    void print() const override;

    /// Get geometry type
    Int_t getType() const { return type; }
    /// Get number of modules
    /// \return number of modules
    Int_t getModules() const { return modules; }
    Float_t getModuleZ(Int_t m) const;
    Int_t getLayers(Int_t m) const;
    Int_t getFibers(Int_t m, Int_t l) const;
    Float_t getLayerRotation(Int_t m, Int_t l) const;
    Float_t getFiberOffsetX(Int_t m, Int_t l) const;
    Float_t getFiberOffsetY(Int_t m, Int_t l) const;
    Float_t getFibersPitch(Int_t m, Int_t l) const;
};

#endif /* SFIBERSGEOMPAR_H */
