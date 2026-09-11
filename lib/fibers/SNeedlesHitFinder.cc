// @(#)lib/fibers:$Id$

#include "SNeedlesHitFinder.h"
#include "SCalContainer.h" // for SCalPar, SCalContainer, SVirtualCal...
#include "SCategory.h"
#include "SDatabase.h"  // for pm, SDatabase
#include "SFibersCal.h" // for SFibersCal
#include "SFibersCalSim.h"
#include "SSiPMCluster.h"
#include "SFibersHit.h" // for SFibersHit
#include "SFibersHitFinderPar.h"
#include "SFibersHitSim.h"
#include "SFibersLookup.h"
#include "SMultiFibersLookup.h"
#include "SPixelDetectorGeomPar.h"
#include "SLocator.h"
#include "SLookup.h"
#include "SPar.h" // for SPar
#include "SiFi.h"

#include <RtypesCore.h>
#include <TObject.h>  // for TObject
#include <TVector3.h> // for TVector3

#include <cstdio>
#include <cstdlib> // for exit, EXIT_FAILURE
#include <iostream>
#include <math.h>
#include <memory> // for allocator

#include <TCanvas.h>
#include <TGraph.h>
#include <TMarker.h>
#include <TString.h>
#include <vector>

/**
 * \class SNeedlesHitFinder
\ingroup lib_fibers

A hit finder task for Needles Stack for 2D CM.

Takes SiPM cluster and creates needle hits.

*/

/**
 * Init task
 *
 * \sa STask::init()
 * \return success
 */
bool SNeedlesHitFinder::init()
{
    // get SiPM cluster category
    catSiPMClus = sifi()->getCategory(SCategory::CatSiPMClus);
    if (!catSiPMClus)
    {
        std::cerr << "No CatSiPMClus category" << std::endl;
        return false;
    }

    // create Hit category
    catFibersHit = sifi()->buildCategory(SCategory::CatFibersHit);
    if (!catFibersHit)
    {
        std::cerr << "No CatFibersHit category" << std::endl;
        return false;
    }

    // get SiPM-needle lookup table
    sipmNeedleLookup = dynamic_cast<SMultiFibersLookupTable*>(pm()->getLookupContainer("9to1SiPMtoFibersLookupTable"));
    if (!sipmNeedleLookup)
    {
        std::cerr << "Parameter container '9to1SiPMtoFibersLookupTable' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // get calibrator parameters (511 positions for each needle)
    pHitFinderNeedlePar = dynamic_cast<SCalContainer<2>*>(pm()->getCalContainer("NeedlesHitFinderNeedlePar"));
    if (!pHitFinderNeedlePar)
    {
        std::cerr << "Calibration container 'NeedlesHitFinderNeedlePar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto def = new SCalPar<2>();
    (*def)[0] = 1.0;
    (*def)[1] = 0.0;

    pHitFinderNeedlePar->setDefault(def);

    // get floodmap positions of the needles
    pFloodmapPositions = dynamic_cast<SCalContainer<2>*>(pm()->getCalContainer("NeedlesFloodmapPos"));
    if (!pFloodmapPositions)
    {
        std::cerr << "Calibration container 'NeedlesFloodmapPos' was not obtained!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    pDetectorGeomPar = dynamic_cast<SPixelDetectorGeomPar*>(pm()->getParContainer("PixelDetectorGeomPar"));
    if (!pDetectorGeomPar)
    {
        std::cerr << "Parameter container 'SPixelDetectorGeomPar' was not obtained!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::vector<std::string>> vec;

    return true;
}

/**
 * Execute task
 *
 * \sa STask::execute()
 * \return success
 */
bool SNeedlesHitFinder::execute()
{
    static int numDrawn = 0; 
    int maxEventsToDraw = 0;

    int size = catSiPMClus->getEntries();
    for (int i = 0; i < size; ++i)
    {
        SSiPMCluster* pCluster = dynamic_cast<SSiPMCluster*>(catSiPMClus->getObject(i));
        if (!pCluster)
        {
            printf("SSiPMCluster doesn't exist!\n");
            continue;
        }

        if (pCluster->getHitsArray().size() < 3) continue; // skip clusters with size < 3, since CoG is not reliable

        Int_t mod = 0;
        Int_t lay = 0;
        Int_t ele = 0;
        char c = ' '; // cluster/side
        pCluster->getAddress(mod, c);

        SLocator loc(3);
        loc[0] = mod;
        loc[1] = lay;
        loc[2] = ele;

        Float_t qdc = pCluster->getQDC();
        Double_t time = pCluster->getTime();

        if (fabs(time) < 0.0001) continue; // if time is small, then sipm hit was not recorded

        TVector3 cog = pCluster->getPoint();

        // hardcoded cuts, only for debugging
        // if (cog.Y() < 0.5 || cog.Y() > 6.5) continue;
        // if (cog.X() >= 18.5 && cog.X() <= 21.5 && cog.Y() >= 4.5 && cog.Y() <= 7.5) continue;

        Int_t sipmElement = std::lround(cog.X());
        Int_t sipmLayer = std::lround(cog.Y());

        Double_t minDistance2 = 1e6;
        Float_t x = 0.;
        Float_t y = 0.;
        Float_t z = 7.5; // TODO change

        Double_t maxAllowedDistX = 0.2;
        Double_t maxAllowedDistY = 0.2;

        bool drawThisEvent = (numDrawn < maxEventsToDraw);
        std::vector<double> f_x, f_y;

        // loop over the floodmap positions of 3x3 SiPMs around the cluster CoG position
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                // current SiPM element and layer
                Int_t searchElement = sipmElement + dx;
                Int_t searchLayer = sipmLayer + dy;

                // skip SiPMs that are outside the valid range of the detector geometry
                if (searchLayer < 0 || searchLayer >= pDetectorGeomPar->getNumSiPMLayers())
                {
                    continue; 
                }

                Int_t maxElementsInLayer = pDetectorGeomPar->getNumSiPMsPerLayer(searchLayer);

                if (searchElement < 0 || searchElement >= maxElementsInLayer)
                {
                    continue;
                }

                Int_t channelID = -1;

                // take the SiPM ID for the current SiPM address
                pDetectorGeomPar->getswSiPMIDFromAddress(channelID, mod, searchLayer, searchElement, c);

                // std::cout << "SNeedlesHitFinder: mod " << mod << ", searchLayer " << searchLayer << ", searchElement " << searchElement << ", side " << c << ", channelID " << channelID << std::endl;

                // get the needles attached to the current SiPM
                auto* sn = dynamic_cast<SMultiFibersChannel*>(sipmNeedleLookup->getAddress(0x1000, channelID));

                if (!sn)
                {
                    std::cerr << "No SiPM-needle lookup for module " << mod << ", layer " << searchLayer << ", element " << searchElement << ", side " << c << std::endl;
                    continue;
                }

                // loop over the needles attached to the current SiPM and find the floodmap position closest to the CoG position
                for (const auto& addr : sn->vecFiberAssociations)
                {
                    SFibersChannel needle;
                    needle.m = std::stoi(addr[0]);
                    needle.l = std::stoi(addr[1]);
                    needle.s = std::stoi(addr[2]);
                    needle.side = addr[3][0];

                    auto* flood = pFloodmapPositions->getPar(&needle);
                    if (!flood)
                    {
                        std::cerr << "No floodmap position for needle (" << needle.m << ", " << needle.l << ", " << needle.s << ", " << needle.side << ")" << std::endl;
                        continue;
                    }

                    // parameters in flood:
                    // [0] - X
                    // [1] - Y

                    if ((*flood)[0] == -100.0 && (*flood)[1] == -100.0)
                    {
                        continue; // skip uncalibrated needles
                    }

                    if (drawThisEvent) {
                        f_x.push_back((*flood)[0]);
                        f_y.push_back((*flood)[1]);
                    }

                    Double_t diffX = std::abs(cog.X() - (*flood)[0]);
                    Double_t diffY = std::abs(cog.Y() - (*flood)[1]);

                    if (diffX > maxAllowedDistX || diffY > maxAllowedDistY) continue; // skip if the distance to a floodmap position is too large

                    // minimise the 2D squared distance to find the closest point
                    Double_t distance2 = diffX * diffX + diffY * diffY;

                    if (distance2 < minDistance2) // comparing squared distances is faster
                    {
                        minDistance2 = distance2;
                        lay = needle.l;
                        ele = needle.s;
                        x = (*flood)[0];
                        y = (*flood)[1];
                    }
                }
            }
        }

        if (minDistance2 == 1e6) continue; // no valid needle found

        // draw debug plots with CoG and floodmap positions for the first few events
        if (drawThisEvent) {
            TCanvas c("c_debug", "CoG vs floodmap check", 800, 800);
            
            // draw all floodmap positions as red circles
            TGraph gFlood(f_x.size(), f_x.data(), f_y.data());
            gFlood.SetMarkerStyle(20); // circle
            gFlood.SetMarkerColor(kRed);
            gFlood.SetTitle("Cluster CoG vs floodmap;X;Y");
            gFlood.Draw("AP"); // 'A' draws axes, 'P' draws markers

            // draw the matched floodmap position as a blue circle
            TMarker mMatched(x, y, 20);
            mMatched.SetMarkerColor(kBlue);
            // mMatched.SetMarkerSize(1.5);
            mMatched.Draw("SAME");

            // draw the cluster CoG (hit position) as a star
            TMarker mCoG(cog.X(), cog.Y(), 29);
            mCoG.SetMarkerColor(kOrange);
            mCoG.SetMarkerSize(2.0);
            mCoG.Draw("SAME");

            // save to current working directory
            c.SaveAs(Form("cog_floodmap_check_%d.pdf", numDrawn));
            numDrawn++;
        }
        
        loc[1] = lay;
        loc[2] = ele;

        int needleID = -100;
        
        pDetectorGeomPar->getswNeedleIDFromAddress(needleID, mod, lay, ele);

        if (std::abs(cog.X()-x) >= 0.2 || std::abs(cog.Y()-y) >= 0.2) std::cout << "CoG: " << cog.X() << ", " << cog.Y() << " -> Needle: " << x << ", " << y << ", layer: " << lay << ", element: " << ele << ", ID: " << needleID << std::endl;

        SFibersHit* pHit = dynamic_cast<SFibersHit*>(catFibersHit->getObject(loc));
        if (!pHit)
        {
            pHit = reinterpret_cast<SFibersHit*>(catFibersHit->getSlot(loc));
            // if (sifi()->isSimulation())
                // pHit = new (pHit) SFibersHitSim;
            // else
                pHit = new (pHit) SFibersHit;
            pHit->Clear();
        }

        pHit->setAddress(mod, lay, ele);

        // set time
        pHit->setTime(time, 1e-3);

        // obtain per fiber hit finder parameters
        SFibersChannel chan;
        chan.m = mod;
        chan.l = lay;
        chan.s = ele;
        chan.side = c;

        // Float_t a0 = 0.0;
        // Float_t lambda = 0.0;
        Float_t alpha = 0.0;

        if (!pHitFinderNeedlePar)
        {
            std::cerr << "No hit finder calibration container available for module " << mod << ", layer " << lay << ", element " << ele << std::endl;
            continue;
        }

        auto* hfp = pHitFinderNeedlePar->getPar(&chan);
        if (!hfp)
        {
            std::cerr << "No hit finder calibration parameters for module " << mod << ", layer " << lay << ", element " << ele << std::endl;
            continue;
        }
        alpha = (*hfp)[0];


        pHit->getPoint().SetXYZ(x, y, z);
        // pHit->getErrors().SetXYZ(s_x, s_y, s_z);
        // pHit->setU(u, 10.);

        // calculate energy
        Float_t E = alpha * qdc;
        // std::cout << "QDC: " << qdc << ", alpha: " << alpha << ", E: " << E << std::endl;
        pHit->setE(E, 0);

        // TODO how to do it with SSiPMCluster?
        // SFibersCalSim* pCalSim = dynamic_cast<SFibersCalSim*>(pCal);
        // if (sifi()->isSimulation() and pCalSim)
        // {
        //     ((SFibersHitSim*)pHit)->setGeantEnergyLoss(pCalSim->getGeantEnergyLoss());
        //     ((SFibersHitSim*)pHit)->getGeantPoint() = pCalSim->getGeantPoint();
        // }
    }

    return true;
}

/**
 * Finalize task
 *
 * \sa STask::finalize()
 * \return success
 */
bool SNeedlesHitFinder::finalize() { return true; }
