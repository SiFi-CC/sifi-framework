// @(#)lib/fibers_stack:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSSTACKCLUSTER_H
#define SFIBERSSTACKCLUSTER_H

#include "sifi_export.h"

#include "SFibersStackHit.h"

#include <TObject.h>

class SIFI_EXPORT SFibersStackCluster : public SFibersStackHit
{
protected:
    // members
    std::vector<int> hits;

public:
    // constructor
    SFibersStackCluster();
    virtual ~SFibersStackCluster() = default;

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

    /// Add cluster component id
    /// \param id hit id
    void addHit(Int_t id) { hits.push_back(id); }

    /// Get cluster components
    /// \return hits id vector
    const std::vector<int> & getHitsArray() { return hits; }

    void print() const;

    ClassDef(SFibersStackCluster, 1);   // container for fibers stack raw data
};

#endif /* SFIBERSSTACKCLUSTER_H */
