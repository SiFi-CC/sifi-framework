// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SSIPMHIT_H
#define SSIPMHIT_H

#include "sifi_export.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h> // for Float_t, Int_t, Option_t
#include <TObject.h>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SSiPMHit : public TObject
{
protected:
    // members
    Int_t module{-1}; ///< address - module
    Int_t layer{-1};  ///< address - layer
    Int_t element{-1};  ///< address - sipm
    char side;  ///< address - side

    Int_t swSiPMID{-1}; ///< SIPM ID

    Float_t qdc{0.};  ///< SiPM qdc value
    Double_t time{0}; ///< SiPM time value
    
    Int_t hitID{-1}; ///< hit ID

public:
    // constructor
    SSiPMHit() = default;
    virtual ~SSiPMHit() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set SiPM ID
    /// \param sID SiPM ID
    void setChannel(Int_t sID)
    {
        swSiPMID = sID;
    }
    /// Get SiPM ID
    /// \param sID SiPM ID
    void getChannel(Int_t& sID) const
    {
        sID = swSiPMID;
    }
    /// Set SiPM hit ID
    /// \param id SiPM hit ID
    void setID(Int_t id)
    {
        hitID = id;
    }
    /// Get SiPM hit ID
    Int_t getID()
    {
        return hitID;
    }
    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f element
    void setAddress(Int_t m, Int_t l, Int_t f, char s)
    {
        module = m;
        layer = l;
        element = f;
        side = s;
    }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t& m, Int_t& l, Int_t& f, char& s) const
    {
        m = module;
        l = layer;
        f = element;
        s = side;
    }

    /// Set QDC value
    /// \param q qdc
    void setQDC(Float_t q) { qdc = q; }
    /// Set Time value
    /// \param t time
    void setTime(Double_t t) { time = t; }

    /// Get QDC value
    /// \return qdc
    Float_t getQDC() const { return qdc; }
    /// Get Time value
    /// \return time
    Double_t getTime() const { return time; }


    virtual void print() const;

    ClassDef(SSiPMHit, 1); // container for SiPM hits raw data
};

#endif /* SSIPMHIT_H */
