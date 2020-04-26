// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKGEOMPAR_H
#define SFIBERSSTACKGEOMPAR_H

#include <TArrayI.h>
#include <TArrayF.h>

#include "SPar.h"

class SFibersStackGeomPar : public SPar
{
protected:
    // members
    Int_t modules;                  ///< number of modules
    struct SingleModule             ///< single module configuration
    {
        Int_t layers;               ///< numbre of layers
        TArrayI fibers;             ///< number of fibers
        TArrayF layer_rotation;     ///< layer rotation around the axis
        TArrayF fiber_offset_x;     ///< offset of the first fiber in the layer
        TArrayF fiber_offset_y;     ///< offset of the layers
        TArrayF fibers_pitch;       ///< fibers pitch in a layer
    };

    SingleModule * mods;            ///< params for each module

public:
    SFibersStackGeomPar();
    virtual ~SFibersStackGeomPar();

    bool getParams(SParContainer * parcont) override;
    bool putParams(SParContainer * parcont) const override;
    void clear() override;
    void print() const override;

    /// Get number of modules
    /// \return number of modules
    Int_t getModules() const { return modules; }
    Int_t getLayers(Int_t m) const;
    Int_t getFibers(Int_t m, Int_t l) const;
    Float_t getLayerRotation(Int_t m, Int_t l) const;
    Float_t getFiberOffsetX(Int_t m, Int_t l) const;
    Float_t getFiberOffsetY(Int_t m, Int_t l) const;
    Float_t getFibersPitch(Int_t m, Int_t l) const;
};

#endif /* SFIBERSSTACKGEOMPAR_H */
