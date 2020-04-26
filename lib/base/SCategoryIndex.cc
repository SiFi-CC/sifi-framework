// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include <iostream>

#include <TBuffer.h>
#include <TArrayI.h>
#include <TClass.h>
#include <TClonesArray.h>

#include "SCategoryIndex.h"

/** \class SCategoryIndex
\ingroup lib_base

An index object for the category. This class provides an interface to map
n-dimensional array into linear array, and compress the array to reduce size
of the linear array to the number of elements.

*/

/// Constructor
SCategoryIndex::SCategoryIndex() : TObject(), compressed(kFALSE)
{
    clear();
}

/**
 * Map uncompressed index pos into compressed index val.
 *
 * \param pos input index of the original coordinate
 * \param val index of the mapped value
 * \return false is the object is compressed and no further indexes can be map
 */
Bool_t SCategoryIndex::setMapIndex(Int_t pos, Int_t val)
{
    if (compressed)
        return kFALSE;

    idxmap[pos] = val;

    return kTRUE;
}

/**
 * Return map index for given position
 *
 * \param pos input index of the original coordinate
 * \return index of the mapped value
 */
Int_t SCategoryIndex::getMapIndex(Int_t pos)
{
    IndexMap::const_iterator it = idxmap.find(pos);
    if (it == idxmap.end())
        return -1;
    return it->second;
}

/**
 * Clear object
 */
void SCategoryIndex::clear()
{
    compressed = kFALSE;
    idxmap.clear();
}

/**
 * Compress the mapped indexes. After the category is compressed to linear array
 * of the number of elements size, the mapping of the indexes must be also
 * updated. The compression is linear though the mapped indexes receive new
 * values based of their order in the map.
 * Set compressed flag to true.
 * 
 * \sa SCategory::compress()
 */
void SCategoryIndex::compress()
{
    Int_t j = 0;
    IndexMap::iterator it = idxmap.begin();
    for (; it != idxmap.end(); ++it)
        it->second = j++;

    compressed = kTRUE;
}

void SCategoryIndex::Streamer(TBuffer &R__b)
{
    // Stream an object of class HLinearCategory.
    if (R__b.IsReading())
    {
        TObject::Streamer(R__b);
        size_t s;
        R__b >> s;
        Int_t a, b;
        idxmap.clear();
        for (size_t i = 0; i < s; ++i)
        {
            R__b >> a >> b;
            idxmap[a] = b;
        }
        R__b >> compressed;
    } else {
        TObject::Streamer(R__b);
        size_t s = idxmap.size();
        R__b << s;
        for (IndexMap::const_iterator it = idxmap.begin(); it != idxmap.end(); ++it)
        {
            Int_t a = it->first;
            Int_t b = it->second;
            R__b << a << b;
        }
        R__b << compressed;
    }
}
