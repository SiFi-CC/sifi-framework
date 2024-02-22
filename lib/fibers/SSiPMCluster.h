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

#include <SCategory.h>

class SIFI_EXPORT SSiPMCluster : public TObject
{
protected: 
    Int_t clusterID{-1}; ///< address - cluster ID
    Int_t module{-1};    ///< module number
    char side{' '};      ///< side
    
    TVector3 point;  ///< cluster position
    TVector3 errors; ///< cluster position errors
//     Long64_t time{-1};    ///< first hit time within cluster
    Double_t time{-1};    ///< first hit time within cluster
    Float_t qdc{-1};
    Float_t aligned_qdc{-1};
    
    std::vector<Int_t> hits; ///< list of hits belonging to the cluster
    
public:
    
    /// Default constructor
    SSiPMCluster() 
    {
        point.SetXYZ(-1, -1, -1);
        errors.SetXYZ(-1, -1, -1);
    };
    /// Default destructor
    virtual ~SSiPMCluster() = default;
    
    /// Clear cluster object
    /// \param opt - clearing options (only for compatibility with ROOT)
    virtual void Clear(Option_t *opt = "") override;
    
    /// Setting the cluster ID
    /// \param m - module
    /// \param c - cluster
    void setID(Int_t c)
    {
        clusterID = c;
    }
    
    /// Getting the cluster ID
    void getID(Int_t &c) const
    {
        c = clusterID;
    }
    
    /// Set address
    /// \param m module
    /// \param c cluster
    void setAddress(Int_t m, char s)
    {
        module = m;
        side = s;
    }
    
    /// Get address
    /// \param m module
    /// \param c cluster
    void getAddress(Int_t& m, char& c) const
    {
        m = module;
        c = side;
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
    
    void setPoint(TVector3 &p) { point = p; };
    
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
    
    void setErrors(TVector3 &e) {errors = e; };
    
    /// Get position errors
    /// \return errors of the cluster position
    TVector3 & getErrors()
    {
        return errors;
    }
    
    /// \copydoc getErrors()
    const TVector3 & getErrors() const
    {
        return errors;
    }
    
    void setTime(Double_t t) { time = t; };
    
    Double_t getTime() { return time; };
    
    void setQDC(float c) { qdc = c; };
    
    float getQDC() { return qdc; };
    
    void setAlignedQDC(float ac) { aligned_qdc = ac; };
    
    float getAlignedQDC() { return aligned_qdc; };
    
    /// Printing details of the object.
    void print() const;
    
    ClassDefOverride(SSiPMCluster, 1);
};

#endif /* SSIPMCLUSTER_H */
