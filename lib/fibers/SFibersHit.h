// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSHIT_H
#define SFIBERSHIT_H

#include "sifi_export.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h> // for Float_t, Int_t, Option_t
#include <TObject.h>
#include <TVector3.h>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersHit : public TObject
{
protected:
    // members
    Int_t module{-1}; ///< address - module
    Int_t layer{-1};  ///< address - layer
    Int_t fiber{-1};  ///< address - fiber

    TVector3 point;  ///< hit point
    TVector3 errors; ///< hit point errors

    Float_t u{0};  ///< u-coord along the fiber
    Float_t su{0}; ///< sigma of u

    Float_t E{0};  ///< Energy
    Float_t sE{0}; ///< sigma of energy

    Long64_t t{0};  ///< time
    Float_t st{0}; ///< sigma of time

public:
    // constructor
    SFibersHit() = default;
    virtual ~SFibersHit() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void setAddress(Int_t m, Int_t l, Int_t f)
    {
        module = m;
        layer = l;
        fiber = f;
    }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t& m, Int_t& l, Int_t& f) const
    {
        m = module;
        l = layer;
        f = fiber;
    }

    /// Set u-hit coordinate reconstructed from times
    /// \param _u u coordinate
    /// \param _su u error sigma
    void setU(Float_t _u, Float_t _su)
    {
        u = _u;
        su = _su;
    }
    /// Set Energy value
    /// \param _E energy
    /// \param _sE sigma of energy
    void setE(Float_t _E, Float_t _sE)
    {
        E = _E;
        sE = _sE;
    }
    /// Set Time value
    /// \param _t time
    /// \param _st sigma of time
    void setTime(Long64_t _t, Float_t _st)
    {
        t = _t;
        st = _st;
    }

    /// Get u-cooridnate
    /// \return ... coordinate
    Float_t getU() const { return u; }
    /// Get sigma of u-coordinate
    /// \return U
    Float_t getUError() const { return su; }

    /// Get Energy value
    /// \return qdc
    Float_t getE() const { return E; }
    /// Get sigma of Energy value
    /// \return energy sigma
    Float_t getEError() const { return sE; }
    /// Get Time value
    /// \return time
    Long64_t getTime() const { return t; }
    /// Get sigma of Time value
    /// \return time sigma
    Float_t getTimeError() const { return st; }

    /// Get point vector
    /// \return hit position
    TVector3& getPoint() { return point; }
    /// Get point vector
    /// \return hit position
    const TVector3& getPoint() const { return point; }

    /// Get point errors vector
    /// \return vector of errors
    TVector3& getErrors() { return errors; }
    /// Get point errors vector
    /// \return vector of errors
    const TVector3& getErrors() const { return errors; }

    virtual void print() const;

    ClassDef(SFibersHit, 1); // container for fibers stack raw data
};

#endif /* SFIBERSHIT_H */
