// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SLOCATOR_H
#define SLOCATOR_H

#include "sifi_export.h"

#include <cstddef>
#include <map>
#include <string>

#include <TArrayI.h>
#include <TClonesArray.h>
#include <TObject.h>
#include <TString.h>

class SIFI_EXPORT SLocator
{
private:
    // members
    std::size_t dim;          ///< dimension of the locator
    std::vector<size_t> addr; ///< cooridnates of the locator

public:
    SLocator() = delete;
    // constructor
    /// Constructor
    /// \param N dimension of the locator
    explicit SLocator(std::size_t N) : dim(N), addr(N) {}
    // destructor
    virtual ~SLocator() = default;

    // methods

    /// Access coordinate at given position.
    /// \param n dimension position (0..N)
    /// \return coordinate
    size_t& operator[](size_t n) { return addr[n]; }
    /// Same as operator[]()
    /// \param n dimension position (0..N)
    /// \return coordinate
    size_t at(size_t n) const { return addr[n]; }
//     bool operator<(const SLocator & l) const
//     {
//         for (size_t i = 0; i < dim; ++i)
//         {
//             if (addr[i] < l.addr[i])
//                 return true;
//             else if (addr[i] > l.addr[i])
//                 return false;
//         }
// 
//         return false;
//     }
// 
//     bool operator>(const SLocator & l)
//     {
//         return l < *this;
//     }

    /// Get locator dimensions
    /// \return number of dimensions
    std::size_t getDim() const { return dim; }

    void print() const;

private:
    ClassDef(SLocator, 1);
};

#endif /* SLOCATOR_H */
