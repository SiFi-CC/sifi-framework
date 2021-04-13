// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SCATEGORYINDEX_H
#define SCATEGORYINDEX_H

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDefOverride
#include <RtypesCore.h> // for Int_t, Bool_t
#include <TObject.h>

#include <cstddef> // for size_t
#include <map>

class TBuffer;
class TClass;
class TMemberInspector;

class SCategoryIndex : public TObject
{
private:
    // members
    std::map<Int_t, Int_t> idxmap; ///< map of indexes
    // flags
    Bool_t compressed; ///< compressed

public:
    SCategoryIndex();
    virtual ~SCategoryIndex() = default;

    Bool_t setMapIndex(Int_t pos, Int_t val);
    Int_t getMapIndex(Int_t pos) const;

    /// Is category compressed already
    /// \return compressed
    Bool_t isCompressed() const { return compressed; }
    /// Get number of categories
    /// \return number
    size_t size() const { return idxmap.size(); }

    void clear();
    void compress();

    ClassDefOverride(SCategoryIndex, 1);
};

#endif /* SCATEGORYINDEX_H */
