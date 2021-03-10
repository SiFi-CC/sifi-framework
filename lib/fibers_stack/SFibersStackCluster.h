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

#include <TObject.h>
#include <TVector3.h>

class SIFI_EXPORT SFibersStackCluster : public TObject
{
protected:
    // members
    Int_t module{-1};  ///< address - module
    Int_t cluster{-1}; ///< address - layer

    TVector3 point;
    TVector3 errors;
    std::vector<int> hits;

public:
    // constructor
    SFibersStackCluster() = default;
    virtual ~SFibersStackCluster() = default;

    // inherited from ROOT
    virtual void Clear(Option_t* opt = "");

    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void setAddress(Int_t m, Int_t c)
    {
        module = m;
        cluster = c;
    }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t& m, Int_t& c) const
    {
        m = module;
        c = cluster;
    }

    /// Add cluster component id
    /// \param id hit id
    void addHit(Int_t id) { hits.push_back(id); }

    /// Get cluster components
    /// \return hits id vector
    const std::vector<int>& getHitsArray() { return hits; }

    // get point vector
    TVector3& getPoint() { return point; }
    const TVector3& getPoint() const { return point; }

    // get point errors vector
    TVector3& getErrors() { return errors; }
    const TVector3& getErrors() const { return errors; }

    void print() const;

    ClassDef(SFibersStackCluster, 1); // container for fibers stack raw data
};

#endif /* SFIBERSSTACKCLUSTER_H */
