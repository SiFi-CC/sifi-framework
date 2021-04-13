// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCATEGORY_H
#define SCATEGORY_H

#include "sifi_export.h"

#include "SCategoryHeader.h"
#include "SCategoryIndex.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDefOverride
#include <RtypesCore.h> // for Int_t, Bool_t, kTRUE
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h> // for TString

#include <cstddef> // for size_t

class SLocator;
class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SCategory : public TObject
{
protected:
    // members
    SCategoryHeader header; ///< header information
    SCategoryIndex index;   ///< index information
    /// holds category data
    TClonesArray* data; //-> data

public:
    /// List of all categories
    enum Cat
    {
        // simulations
        CatGeantTrack = 0, ///< geant track
        CatGeantFibersRaw, ///< fibers hit
                           // helpers
        CatDDSamples = 10,
        // fibers
        CatFibersRaw = 20, ///< fibers stack raw data
        CatFibersCal,      ///< fibers cal data
        CatFibersHit,      ///< fibers hit
        CatFibersClus,     ///< fibers cluster
                           // tracks
                           // Limit
        CatLimitDoNotUse,  ///< holds size of the category list
        CatNone,           ///< Clear category list in SLoop
    };

    // constructors
    SCategory();
    SCategory(const char* name, size_t dim, size_t* sizes, bool simulation);
    // destructor
    virtual ~SCategory();

    // methods
    //     TObject * operator[](const SLocator & n);
    TObject*& getSlot(const SLocator& n);
    TObject*& getNewSlot();
    TObject* getObject(const SLocator& n);
    TObject* getObject(Int_t i);

    /// Returns name of the container
    /// \return container name
    TString getName() const { return header.name; }

    /// Returns number of entries in the category
    /// \return number of entries
    Int_t getEntries() const { return data->GetEntries(); }

    /// \sa TObject::IsFolder()
    /// \return is a folder
    Bool_t IsFolder() const override { return kTRUE; }

    void compress();

    void clear();
    void print() const;

private:
    void setup(const char* name, size_t dim, size_t* sizes, bool simulation);

    bool checkDim(const SLocator& loc);
    int loc2pos(const SLocator& loc);

private:
    ClassDefOverride(SCategory, 1);
};

#endif /* SCATEGORY_H */
