// @(#)lib/geant:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SGeantTrack.h"
/** \class SGeantTrack
\ingroup lib_geant

A container for Geant tracks

*/

/** Constructor
 */
SGeantTrack::SGeantTrack()
{
    clear();
}

/** Clear container
 */
void SGeantTrack::clear()
{
    TVector3 p(0,0,0);

    start.x = 0.;
    start.y = 0.;
    start.z = 0.;
    start.px = 0.;
    start.py = 0.;
    start.pz = 0.;
    start.E = -1000.;

    stop.x = 0.;
    stop.y = 0.;
    stop.z = 0.;
    stop.px = 0.;
    stop.py = 0.;
    stop.pz = 0.;
    stop.E = -1000.;

    scattering                          = false;
    processes.clear();
    g4Id                                = 0;
    stopInDetector                      = false;
    secondariesID.clear();
    trackID                             = 0;
    parentID                            = -1;
    generationProcess                   = "";
    inAcceptance                        = kFALSE;
    proc_arr[COMPTON]                   = kFALSE;
    proc_arr[INELASTIC]                 = kFALSE;
    proc_arr[ATREST]                    = kFALSE;
}

/** Add process
 * \param name process name;
 */
void SGeantTrack::addProcess(const std::string & name)
{
    processes.push_back(name);
}

/** Add child ID
 * \param ID child ID
 */
void SGeantTrack::addChildID(Int_t ID)
{
    secondariesID.push_back(ID);
}

/** Get dostance from the reference of the stop point
 * \param ref reference point
 * \return distance (mm)
 */
Double_t SGeantTrack::getDistance(const TVector3 & ref) const
{
    TVector3 end_ = TVector3(stop.x, stop.y, stop.z) - ref;
    return end_.Mag();
}

/** Get range in the detector
 * \param ref reference
 * \return range
 */
Double_t SGeantTrack::getRange(const TVector3 & ref) const
{
    if (!stopInDetector)
        return -100.;

    TVector3 sta = TVector3(start.x, start.y, start.z) - ref;
    TVector3 sto = TVector3(stop.x, stop.y, stop.z) - ref;

    TVector3 psta = TVector3(start.px, start.py, start.pz);
//     TVector3 psto = TVector3(stop.px, stop.py, stop.pz);

    psta *= 1.0/psta.Mag();
//     psto *= 1.0/psto.Mag();

    Double_t t = 0.0;

    const Double_t limit = ref.Y();
    while (true)
    {
        if (
            (fabs(sta.X() + t * psta.X()) < limit) and
            (fabs(sta.Y() + t * psta.Y()) < limit) and
            (fabs(sta.Z() + t * psta.Z()) < limit) )
        {
            break;
        }

        t += 0.1;
    }

    TVector3 intsec(sta + psta * t);

//     printf("start = %f,%f,%f\n", sta.X(), sta.Y(), sta.Z());
//     printf("  end = %f,%f,%f\n", end.X(), end.Y(), end.Z());
//     printf("  int = %f,%f,%f\n", intsec.X(), intsec.Y(), intsec.Z());
    return (intsec - sto).Mag();
}

/** Print track
 */
void SGeantTrack::print() const
{
    TVector3 psta = TVector3(start.px, start.py, start.pz);
    TVector3 psto = TVector3(stop.px, stop.py, stop.pz);

    psta *= 1.0/psta.Mag();
    psto *= 1.0/psto.Mag();

    printf("##### particle #####\n");
    printf("  pos sta=(%f,%f,%f)  sto=(%f,%f,%f)\n", start.x, start.y, start.z, stop.x, stop.y, stop.z);
    printf("  dir sta=(%f,%f,%f)  sto=(%f,%f,%f)\n", psta.X(), psta.Y(), psta.Z(), psto.X(), psto.Y(), psto.Z());
    printf("  start energy = %f,  stop energy = %f\n", start.E, stop.E);
    printf("  scat=%d  process=", scattering);
    for (Int_t i = 0; i < processes.size(); ++i)
        printf("%s,", processes[i].c_str());
    printf("\n");
    printf("  PID=%ld  stop in det=%d\n", g4Id, stopInDetector);
    printf("  num of sec=%zu\n", secondariesID.size());
    //   std::vector<Int_t> secondaries_ID;
    //   Int_t particle_ID;
    //   string generationProcess;
    //   Double_t startEnergy;
    //   Double_t endEnergy;
}

/** Clear object.
 * Parameter options are ignored, for ROOT compatibility.
 * \param opt options
 */
void SGeantTrack::Clear(Option_t* opt)
{
    clear();
}
