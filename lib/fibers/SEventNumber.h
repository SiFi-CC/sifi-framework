// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SEVENTNUMBER_H
#define SEVENTNUMBER_H

#include "sifi_export.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h> // for Float_t, Int_t, Option_t
#include <TObject.h>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SEventNumber : public TObject
{
protected:
    // members
    Int_t eventNumber{-1}; ///< event number
public:
    // constructor
    SEventNumber() = default;
    virtual ~SEventNumber() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods

    void setEventNumber(Int_t evNumber)
    {
        eventNumber = evNumber;
    }

    void getEventNumber(Int_t& evNumber) const
    {
        evNumber = eventNumber;
    }

    virtual void print() const;

    ClassDef(SEventNumber, 1);
};

#endif /* SEVENTNUMBER_H */

