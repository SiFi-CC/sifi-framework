// @(#)lib/fibers:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SFibersClusterFinder.h"
#include "SCategory.h"
#include "SDatabase.h" // for pm, SDatabase
#include "SFibersCluster.h"
#include "SFibersClusterFinderPar.h"
#include "SFibersGeomPar.h"
#include "SFibersHit.h"
#include "SLocator.h"
#include "SPar.h" // for SPar
#include "SiFi.h"

#include <RtypesCore.h>
#include <TObject.h>  // for TObject
#include <TVector3.h> // for TVector3, operator+, operator-

#include <algorithm> // for max
#include <cmath>
#include <cstdio>
#include <cstdlib> // for exit, EXIT_FAILURE
#include <iostream>
#include <map>     // for _Rb_tree_iterator, map, operator!=
#include <memory>  // for allocator_traits<>::value_type
#include <utility> // for pair
#include <vector>  // for vector

/**
 * \class SFibersClusterFinder
\ingroup lib_fibers

A hit finder task for Fibers Stack.

Takes each fiber data and tries to reconstruct hit along the fiber.

*/

/**
 * Test whether the two hits overlaps withing the errors and tolerance.
 * \param a first hits
 * \param b second hit
 * \param tolerance
 * \return overalp
 */
bool check_overlap(SFibersHit* a, SFibersHit* b, Float_t tolerance)
{
    // calculate difference and make it sositive
    TVector3 dpos = a->getPoint() - b->getPoint();
    dpos.SetXYZ(fabs(dpos.X()), fabs(dpos.Y()), fabs(dpos.Z()));

    // maximal overlap range with tolerance included
    TVector3 dsigma = a->getErrors() + b->getErrors() + TVector3(tolerance, tolerance, tolerance);

    // compare each dimensions, value > 0 means no overlap
    dpos -= dsigma;

    if (dpos.X() > 0) return false;
    if (dpos.Y() > 0) return false;
    if (dpos.Z() > 0) return false;
    return true;
}

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SFibersClusterFinder::init()
{
    // get Cal category
    catFibersHit = sifi()->getCategory(SCategory::CatFibersHit);
    if (!catFibersHit)
    {
        std::cerr << "No CatFibersHit category" << std::endl;
        return false;
    }

    // create Cluster category
    catFibersCluster = sifi()->buildCategory(SCategory::CatFibersClus);
    if (!catFibersCluster)
    {
        std::cerr << "No CatFibersCluster category" << std::endl;
        return false;
    }

    // get cluster finder parameters
    pClusterFinderPar =
        dynamic_cast<SFibersClusterFinderPar*>(pm()->getParContainer("FibersClusterFinderPar"));
    if (!pClusterFinderPar)
    {
        std::cerr << "Parameter container 'SFibersClusterFinderPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    pGeomPar = dynamic_cast<SFibersGeomPar*>(pm()->getParContainer("FibersGeomPar"));
    if (!pGeomPar)
    {
        std::cerr << "Parameter container 'SFibersGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

/**
 * Execute task
 *
 * \sa STask::execute()
 * \return success
 */
bool SFibersClusterFinder::execute()
{
    std::vector<std::vector<double>> points;

    const size_t max_mod = pGeomPar->getModules();

    struct Cluster
    {
        int id{-1};
        TVector3 pos;
        TVector3 err;
        Float_t e{0.0};
        std::vector<SFibersHit*> hits;
    };
    std::vector<Cluster> clusters;
    std::map<SFibersHit*, int> hit_cluster_map; // inr = -1: no cluster assigned, id >= cluster id;

    int size = catFibersHit->getEntries();
    for (int i = 0; i < size; ++i)
    {
        SFibersHit* pHit = dynamic_cast<SFibersHit*>(catFibersHit->getObject(i));
        if (!pHit)
        {
            printf("FibersHit doesn't exist!\n");
            continue;
        }
        Int_t mod = 0;
        Int_t lay = 0;
        Int_t fib = 0;
        pHit->getAddress(mod, lay, fib);

        hit_cluster_map[pHit] = -1;
    }

    int cluster_id = -1;
    int unassigned = hit_cluster_map.size();
    const float tolerance = 0.1;

    for (auto h = hit_cluster_map.begin(); h != hit_cluster_map.end(); ++h)
    {
        // if hit doesn't belong to any cluter, create one

        if (h->second == -1)
        {
            h->second = ++cluster_id;

            // creating new cluster
            Cluster clus;
            clus.id = cluster_id;
            clus.hits.push_back(h->first);
            clusters.push_back(clus);

            --unassigned;
        }
        else
            continue;

        // Loop over other hits trying to match them, repeat a many times as not assigned hits yet.
        // First go over other hits, and try to mach them with any hit in the cluster. If overlap
        // found, assign hit to cluster, and continue
        for (int i = 0; i < unassigned; ++i)
        {
            for (auto hit_nr = 0; hit_nr < clusters[cluster_id].hits.size(); ++hit_nr)
            {
                for (auto h2 = h; h2 != hit_cluster_map.end(); ++h2)
                {

                    if (h2->second != -1) continue; // skip assigned hits

                    if (check_overlap(clusters[cluster_id].hits[hit_nr], h2->first, tolerance))
                    {
                        // overlap found
                        h2->second = cluster_id; // assign cluster to hit
                        clusters[cluster_id].hits.push_back(h2->first);
                        --unassigned;
                    }
                }
            }
        }
    }

    // printf(" => hits assigned\n");
    // for (auto h : hit_cluster_map) {
    //     printf("  %#lx -> %d\n", h.first, h.second);
    // }

    int mode = pClusterFinderPar->getClusterMode();
    int clus_cnt[max_mod];
    for (int i = 0; i < max_mod; ++i)
        clus_cnt[i] = 0;

    for (int i = 0; i < clusters.size(); ++i)
    {
        float hf_e = 0.;          // temp variable for HF mode highest energy
        float ff_z = 1.e12;       // temp variable for FF for minimal Z
        float disc_x_min = 1.e12; // x-spread for discrete X
        float disc_x_max = -1.e12;
        float disc_z_min = 1.e12; // z-spread for discrete Z
        float disc_z_max = -1.e12;

        TVector3 weight_sum(0, 0, 0);

        for (auto h : clusters[i].hits)
        {

            if (mode == 0)
            { // AC
                TVector3 np = h->getPoint();

                // finding spread
                Float_t curr_x = np.X();
                if (curr_x < disc_x_min) disc_x_min = curr_x;
                if (curr_x > disc_x_max) disc_x_max = curr_x;
                Float_t curr_z = np.Z();
                if (curr_z < disc_z_min) disc_z_min = curr_z;
                if (curr_z > disc_z_max) disc_z_max = curr_z;

                TVector3 e = h->getErrors();
                TVector3 weight(h->getE() / e.X() / e.X(), h->getE() / e.Y() / e.Y(),
                                h->getE() / e.Z() / e.Z());

                np.SetX(np.X() * weight.X());
                np.SetY(np.Y() * weight.Y());
                np.SetZ(np.Z() * weight.Z());

                clusters[i].pos += np;
                clusters[i].err += weight;
                weight_sum += weight;
            }
            else if (mode == 1)
            { // HF
                TVector3 np = h->getPoint();

                // finding spread
                Float_t curr_x = np.X();
                if (curr_x < disc_x_min) disc_x_min = curr_x;
                if (curr_x > disc_x_max) disc_x_max = curr_x;
                Float_t curr_z = np.Z();
                if (curr_z < disc_z_min) disc_z_min = curr_z;
                if (curr_z > disc_z_max) disc_z_max = curr_z;

                TVector3 e = h->getErrors();
                TVector3 weight(h->getE() / e.X() / e.X(), h->getE() / e.Y() / e.Y(), 0);

                np.SetX(np.X() * weight.X());
                np.SetY(np.Y() * weight.Y());
                np.SetZ(0);

                clusters[i].pos += np;
                clusters[i].err = weight;
                weight_sum += weight;

                if (h->getE() > hf_e)
                {
                    hf_e = h->getE();
                    clusters[i].pos.SetZ(h->getPoint().Z());
                }
                else
                {
                }
            }
            else if (mode == 2)
            { // FF
                TVector3 np = h->getPoint();

                // finding spread
                Float_t curr_x = np.X();
                if (curr_x < disc_x_min) disc_x_min = curr_x;
                if (curr_x > disc_x_max) disc_x_max = curr_x;
                Float_t curr_z = np.Z();
                if (curr_z < disc_z_min) disc_z_min = curr_z;
                if (curr_z > disc_z_max) disc_z_max = curr_z;

                TVector3 e = h->getErrors();

                TVector3 weight(h->getE() / e.X() / e.X(), h->getE() / e.Y() / e.Y(), 0);

                np.SetX(np.X() * weight.X());
                np.SetY(np.Y() * weight.Y());
                np.SetZ(0);

                clusters[i].pos += np;
                clusters[i].err = weight;
                weight_sum += weight;

                if (h->getPoint().Z() < ff_z)
                {
                    ff_z = h->getPoint().Z();
                    clusters[i].pos.SetZ(ff_z);
                }
                else
                {
                }
            }
            else
            {
                printf("Wrong clustering mode!\n");
            }

            clusters[i].e += h->getE();
        }

        if (mode == 0)
        { // AC
            clusters[i].pos.SetX(clusters[i].pos.X() / weight_sum.X());
            clusters[i].pos.SetY(clusters[i].pos.Y() / weight_sum.Y());
            clusters[i].pos.SetZ(clusters[i].pos.Z() / weight_sum.Z());

            switch (pGeomPar->getType())
            {
                case 0:                                                      // aligned fibers
                    clusters[i].err.SetX(disc_x_max - disc_x_min + 1. / 2.); // FIXME
                    clusters[i].err.SetY(1. / sqrt(clusters[i].err.Y()));
                    clusters[i].err.SetZ(disc_z_max - disc_z_min + 1. / 2.); // FIXME
                    break;
                case 1: // crossed fibers
                case 10:
                    clusters[i].err.SetX(1. / sqrt(clusters[i].err.X()));
                    clusters[i].err.SetY(1. / sqrt(clusters[i].err.Y()));
                    clusters[i].err.SetZ(disc_z_max - disc_z_min + 1. / 2.); // FIXME
                    break;
                default:
                    printf("Unsupported geometry type!\n");
            }
        }
        else if (mode == 1 or mode == 2)
        { // HF
            clusters[i].pos.SetX(clusters[i].pos.X() / weight_sum.X());
            clusters[i].pos.SetY(clusters[i].pos.Y() / weight_sum.Y());

            switch (pGeomPar->getType())
            {
                case 0:
                    clusters[i].err.SetX(disc_x_max - disc_x_min + 1. / 2.); // FIXME
                    clusters[i].err.SetY(1. / sqrt(clusters[i].err.Y()));
                    clusters[i].err.SetZ(disc_z_max - disc_z_min + 1. / 2.); // FIXME
                    break;
                case 1: // crossed fibers
                case 10:
                    clusters[i].err.SetX(1. / sqrt(clusters[i].err.X()));
                    clusters[i].err.SetY(1. / sqrt(clusters[i].err.Y()));
                    clusters[i].err.SetZ(disc_z_max - disc_z_min + 1. / 2.); // FIXME
                    break;
                default:
                    printf("Unsupported geometry type!\n");
            }
        }
        else
        {
            printf("Wrong clustering mode!\n");
        }

        int m, l, f;
        clusters[i].hits[0]->getAddress(m, l, f);

        SLocator loc(2);
        loc[0] = m;
        loc[1] = clus_cnt[m]++;

        SFibersCluster* pCluster =
            reinterpret_cast<SFibersCluster*>(catFibersCluster->getSlot(loc));
        if (pCluster) pCluster = new (pCluster) SFibersCluster;

        if (pCluster)
        {
            pCluster->setAddress(loc[0], loc[1]);
            pCluster->getPoint() = clusters[i].pos;
            pCluster->getErrors() = clusters[i].err;
        }
        else
        {
            printf("Cluster of m=%ld with id=%ld could not be add.\n", loc[0], loc[1]);
        }
    }

    return true;
}

/**
 * Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SFibersClusterFinder::finalize() { return true; }
