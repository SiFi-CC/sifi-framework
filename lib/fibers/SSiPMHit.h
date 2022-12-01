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
    Int_t SiPMID{-1}; ///< SIPM ID

    Float_t qdc{0.};  ///< SiPM qdc value
    Long64_t time{0}; ///< SiPM time value

public:
    // constructor
    SSiPMHit() = default;
    virtual ~SSiPMHit() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set SiPM ID
    /// \param sID SiPM ID
    void setAddress(Int_t sID)
    {
        SiPMID = sID;
    }
    /// Get SiPM ID
    /// \param sID SiPM ID
    void getAddress(Int_t& sID) const
    {
        sID = SiPMID;
    }

    /// Set QDC value
    /// \param q qdc
    void setQDC(Float_t q) { qdc = q; }
    /// Set Time value
    /// \param t time
    void setTime(Long64_t t) { time = t; }

    /// Get QDC value
    /// \return qdc
    Float_t getQDC() const { return qdc; }
    /// Get Time value
    /// \return time
    Long64_t getTime() const { return time; }


    virtual void print() const;

    ClassDef(SSiPMHit, 1); // container for SiPM hits raw data
};

#endif /* SSIPMHIT_H */
