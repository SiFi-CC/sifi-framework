// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSCLUSTER_H
#define SFIBERSCLUSTER_H

#include "sifi_export.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h> // for Int_t, Option_t
#include <TObject.h>
#include <TVector3.h>

#include <algorithm> // for max
#include <vector>    // for vector

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersCluster : public TObject
{
protected:
    // members
    Int_t module{-1};  ///< address - module
    Int_t cluster{-1}; ///< address - layer

    TVector3 point;        ///< cluster position
    TVector3 errors;       ///< cluster position errors
    std::vector<int> hits; ///< hits belonging to the cluster

public:
    // constructor
    SFibersCluster() = default;
    virtual ~SFibersCluster() = default;

    /// Clear cluster object
    /// \param opt clear options
    virtual void Clear(Option_t* opt = "") override;

    /// Set address
    /// \param m module
    /// \param c cluster
    void setAddress(Int_t m, Int_t c)
    {
        module = m;
        cluster = c;
    }
    /// Get address
    /// \param m module
    /// \param c cluster
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

    /// Get cluster position
    /// \return cluster position
    TVector3& getPoint() { return point; }
    /// \copydoc getPoint()
    const TVector3& getPoint() const { return point; }

    /// Get position errors
    /// \return position errors
    TVector3& getErrors() { return errors; }
    /// \copydoc getErrors()
    const TVector3& getErrors() const { return errors; }

    void print() const;

    ClassDefOverride(SFibersCluster, 1); // container for fibers stack raw data
};

#endif /* SFIBERSCLUSTER_H */
