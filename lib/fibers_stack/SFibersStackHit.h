// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKHIT_H
#define SFIBERSSTACKHIT_H

#include "sifi_export.h"

#include <TObject.h>

class SIFI_EXPORT SFibersStackHit : public TObject
{
protected:
    // members
    Int_t module;       ///< address - module
    Int_t layer;        ///< address - layer
    Int_t fiber;        ///< address - fiber

    Float_t x;          ///< x-coord in the lab system, reconstructed from M_LR
    Float_t y;          ///< y-coord in the lab system
    Float_t z;          ///< z-coord in the lab system
    Float_t xt;         ///< x-coord in the lab system, reconstructed from times

    Float_t sx;         ///< sigma of x
    Float_t sy;         ///< sigma of y
    Float_t sz;         ///< sigma of z
    Float_t sxt;        ///< sigma of xt

    Float_t E;          ///< Energy
    Float_t sE;         ///< sigma of energy

    Float_t t;          ///< time
    Float_t st;         ///< sigma of time

public:
    // constructor
    SFibersStackHit();
    virtual ~SFibersStackHit() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void setAddress(Int_t m, Int_t l, Int_t f) { module = m; layer = l; fiber = f; }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t & m, Int_t & l, Int_t & f) const { m = module; l = layer; f = fiber; }

    /// Set hit coordinates
    /// \param _x x coordinate
    /// \param _y x coordinate
    /// \param _z x coordinate
    void setXYZ(Float_t _x, Float_t _y, Float_t _z) { x = _x; y = _y; z = _z; }
    /// Set x-hit coordinate reconstructed from times
    /// \param _xt x coordinate
    void setXt( Float_t _xt) { xt = _xt; }
    /// Set hit coordinate errors
    /// \param _sx x-coordinate error
    /// \param _sy x-coordinate error
    /// \param _sz x-coordinate error
    void setXYZErrors(Float_t _sx, Float_t _sy, Float_t _sz) { sx = _sx; sy = _sy; sz = _sz; }
    /// Set hit xt-coordinate errors
    /// \param _sxt xt-coordinate error
    void setXtError( Float_t _sxt) { sxt = _sxt; }
    /// Set Energy value
    /// \param _E energy
    /// \param _sE sigma of energy
    void setE(Float_t _E, Float_t _sE) { E = _E; sE = _sE; }
    /// Set Time value
    /// \param _t time
    /// \param _st sigma of time
    void setTime(Float_t _t, Float_t _st) { t = _t; st = _st; }

    /// Get x-cooridnate
    /// \return transverse coordinate
    Float_t getX() const { return x; }
    /// Get y-cooridnate
    /// \return longnitudal coordinate
    Float_t getY() const { return y; }
    /// Get z-cooridnate
    /// \return ... coordinate
    Float_t getZ() const { return z; }
    /// Get z-cooridnate
    /// \return ... coordinate
    Float_t getXt() const { return xt; }
    /// Get XYZ errors
    /// \param _sx sigma of x
    /// \param _sy sigma of y
    /// \param _sz sigma of z
    void getXYZErrors(Float_t & _sx, Float_t & _sy, Float_t & _sz) const { _sx = sx; _sy = sy; _sz = sz; }
    /// Get Xt errors
    /// \return xt error
    Float_t getXtError() const { return sxt; }
    /// Get Energy value
    /// \return qdc
    Float_t getE() const { return E; }
    /// Get sigma of Energy value
    /// \return energy sigma
    Float_t getEError() const { return sE; }
    /// Get Time value
    /// \return time
    Float_t getTime() const { return t; }
    /// Get sigma of Time value
    /// \return time sigma
    Float_t getTimeError() const { return sE; }

    void print() const;

    ClassDef(SFibersStackHit, 1);   // container for fibers stack raw data
};

#endif /* SFIBERSSTACKHIT_H */
