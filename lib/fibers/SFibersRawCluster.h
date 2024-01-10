// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SFIBERSRAWCLUSTER_H
#define SFIBERSRAWCLUSTER_H

#include "sifi_export.h"

#include <Rtypes.h>     // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h> // for Float_t, Int_t, Option_t
#include <TObject.h>

#include <iostream>
#include <vector>

class TBuffer;
class TClass;
class TMemberInspector;

class SIFI_EXPORT SFibersRawCluster : public TObject
{
protected:
    // members
    Int_t module{-1}; ///< address - module
    Int_t layer{-1};  ///< address - layer
    Int_t fiber{-1};  ///< address - fiber

    Float_t qdc_l{0.};  ///< qdc value
    Long64_t time_l{0}; ///< time value

    Float_t qdc_r{0.};  ///< qdc value
    Long64_t time_r{0}; ///< time value

    std::vector<Int_t> sipm_clusters; ///< clusters: 0 - top, 1 - bottom
    Int_t fiberClusterLabel; ///< label of fiber cluster: 
    Int_t fiberMultInCluster; ///< multiplicity of fibers in a cluster
    Int_t nFiberClusters; ///< # of fiber clusters
    
public:
    // constructor
    SFibersRawCluster() = default;
    virtual ~SFibersRawCluster() = default;

//     // inherited from ROOT
//     virtual void Clear(Option_t* opt = "");

    // methods
    /// Set address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void setAddress(Int_t m, Int_t l, Int_t f)
    {
        module = m;
        layer = l;
        fiber = f;
    }
    /// Get address
    /// \param m module
    /// \param l layer
    /// \param f fiber
    void getAddress(Int_t& m, Int_t& l, Int_t& f) const
    {
        m = module;
        l = layer;
        f = fiber;
    }

    void setClusters(Int_t cluster_top, Int_t cluster_bottom)
    {
        sipm_clusters.push_back(cluster_top);
        sipm_clusters.push_back(cluster_bottom);
    }
    
    void getClusters(Int_t &cluster_top, Int_t &cluster_bottom)
    {
        
        if(sipm_clusters.empty())
        {
            std::cerr << "Error in " << __func__ << std::endl;
            std::cerr << "sipm_clusters vector is empty!" << std::endl;
        }
        
        cluster_top = sipm_clusters[0];
        cluster_bottom = sipm_clusters[1];
    }
    
    
//     /// Add Fiber to the cluster
//     /// \param fib_id - Fiber ID
//     void addFiber(Int_t fib_id)
//     {
//         hits.push_back(fib_id);
//     }
    
    /// Set ADC value
    /// \param ql qdc on the left side of the fiber
    /// \param qr qdc on the right side of teh fiber
    void setQDC(Float_t ql, Float_t qr)
    {
        qdc_l = ql;
        qdc_r = qr;
    }
    /// Set Time value
    /// \param tl time on the left side of fiber
    /// \param tr time on the right side of fiber
//     void setTime(Float_t tl, Float_t tr)
//     {
//         time_l = tl;
//         time_r = tr;
//     }
    
    void setTime(Long64_t tl, Long64_t tr)
    {
        time_l = tl;
        time_r = tr;
    }
    /// Set QDC value
    /// \param q qdc
    void setQDCL(Float_t q) { qdc_l = q; }
    /// Set Time value
    /// \param t time
//     void setTimeL(Float_t t) { time_l = t; }
    void setTimeL(Long64_t t) { time_l = t; }
    /// Set QDC value
    /// \param q qdc
    void setQDCR(Float_t q) { qdc_r = q; }
    /// Set Time value
    /// \param t time
//     void setTimeR(Float_t t) { time_r = t; }
    void setTimeR(Long64_t t) { time_r = t; }
    /// Set fiber label value
    /// \param lab label
    void setFiberClusterLabel(Int_t lab) { fiberClusterLabel = lab; }
    /// Set fiber multiplicity value
    /// \param mult multiplicity
    void setFiberMultInCluster(Int_t mult) { fiberMultInCluster = mult; }
    /// Get number of fibers in a cluster
    /// \param nfibers  nFiberClusters
    void setNFibersClusters(Int_t nfibers) { nFiberClusters = nfibers; }
    /// Get QDC value
    /// \return qdc
    Float_t getQDCL() const { return qdc_l; }
    /// Get Time value
    /// \return time
    Long64_t getTimeL() const { return time_l; }
    /// Get QDC value
    /// \return qdc
    Float_t getQDCR() const { return qdc_r; }
    /// Get Time value
    /// \return time
    Long64_t getTimeR() const { return time_r; }
    /// Get fiber label value
    /// \return label
    Int_t getFiberClusterLabel() const { return fiberClusterLabel; }
    /// Get fiber multiplicity value
    /// \return multiplicity
    Int_t getFiberMultInCluster() const { return fiberMultInCluster; }
    /// Get number of fibers in a cluster
    /// \return nFiberClusters
    Int_t getNFibersClusters() const { return nFiberClusters; }
    
//     virtual void print() const;  //causes errors:  bin/ld: ../lib/libFibers.so.0.3.0: undefined reference to `vtable for SFibersRawCluster'
                                    //  //    /bin/ld: ../lib/libFibers.so.0.3.0: undefined reference to `typeinfo for SFibersRawCluster'

    ClassDef(SFibersRawCluster, 1); // container for fibers stack raw cluster data
};

#endif /* SFIBERSRAWCLUSTER_H */
