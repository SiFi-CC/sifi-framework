// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/
 
#ifndef SSIPMCLUSTER_H
#define SSIPMCLUSTER_H

#include "sifi_export.h"

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TObject.h>
#include <TVector3.h>

#include <algorithm>
#include <vector>

class SIFI_EXPORT SSiPMCluster : public TObject
{
    
protected: 
//    Int_t module{-1};  ///< address - module
//    Int_t cluster{-1}; ///< address - cluster ID
    
    TVector3 point;  ///< cluster position
//    TVector3 errors; ///< cluster position errors
    
    std::vector<Int_t> hits; ///< list of hits belonging to the cluster
    
public:
    
    /// Default constructor
    SSiPMCluster() = default;
    /// Default destructor
    virtual ~SSiPMCluster() = default;
    
    /// Clear cluster object
    /// \param opt - clearing options (only for compatibility with ROOT)
    virtual void Clear(Option_t *opt = "") override;
    
    /// Setting the cluster address
    /// \param m - module
    /// \param c - cluster
    void setAddress(Int_t m, Int_t c)
    {
//        module = m;
//        cluster = c;
    }
    
    /// Getting the cluster address
    void getAddress(Int_t &m, Int_t &c) const
    {
//        m = module;
//        c = cluster;
    }
    
    /// Add SiPM hit to the cluster
    /// \param hit_id - SiPM hit ID
    void addHit(Int_t hit_id)
    {
        hits.push_back(hit_id);
    }
    
    /// Get list of SiPM hits in the cluster
    /// \return vector of SiPM hits
    const std::vector<Int_t> & getHitsArray()
    {
        return hits;
    }
    
    /// Get cluster position
    /// \return position of the cluster
    TVector3 & getPoint() 
    {
        return point;
    }
    
    /// \copydoc getPoint()
    const TVector3 & getPoint() const
    {
        return point;
    }
    
    /// Get position errors
    /// \return errors of the cluster position
    TVector3 & getErrors()
    {
//        return errors;
    }
    
    /// \copydoc getErrors()
    const TVector3 & getErrors() const
    {
//        return errors;
    }
    
    /// Printing details of the object.
    void print() const;
    
    ClassDefOverride(SSiPMCluster, 1);
};

#endif /* SSIPMCLUSTER_H */
