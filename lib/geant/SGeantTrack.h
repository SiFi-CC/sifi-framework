// @(#)lib/geant:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SGEANTTRACK_H
#define SGEANTTRACK_H

#include <string>
#include <fstream>
#include "TObject.h"
#include "TRandom2.h"
#include "TVector3.h"

class SGeantTrack : public TObject
{
public:
    /// List of recorderd physics processes
    enum Process {
        COMPTON,                        ///< COmpton scaterring
        INELASTIC,                      ///< Inelastic scattering
        ATREST                          ///< Stopped at rest
    };

    /// Track interation point physical values
    struct Point {
        Float_t x;                      ///< x coordinate
        Float_t y;                      ///< y coordinate
        Float_t z;                      ///< z coordinate
        Float_t px;                     ///< px momentum
        Float_t py;                     ///< py momentum
        Float_t pz;                     ///< pz momentum
        Float_t E;                      ///< total energy
    };

protected:
    Point start;                        ///< start point
    Point stop;                         ///< stop point
    Bool_t scattering;                  ///< was scattering
    std::vector<std::string> processes; ///< processes
    Long_t g4Id;                        ///< Geant ID
    Bool_t stopInDetector;              ///< stop in detector
    std::vector<Int_t> secondariesID;   ///< secondaries PID
    Int_t trackID;                      ///< track ID
    Int_t parentID;                     ///< parent ID
    std::string generationProcess;      ///< generation process
    Bool_t inAcceptance;                ///< in acceptance

    Bool_t proc_arr[ATREST+1];          ///< process list

    TRandom2 randGen;                   ///< random generator

public:
    // constructor
    SGeantTrack();
    // destructor
    virtual ~SGeantTrack() = default;

    void clear();
    void addProcess(const std::string & name);
    void addChildID(Int_t ID);

    /// Set start point
    /// \param x x
    /// \param y y
    /// \param z z
    void setStartXYZ(Float_t x, Float_t y, Float_t z) { start.x = x; start.y = y; start.z = z; }
    /// Get start x
    /// \return x
    Float_t getStartX() const { return start.x; }
    /// Get start y
    /// \return y
    Float_t getStartY() const { return start.y; }
    /// Get start z
    /// \return z
    Float_t getStartZ() const { return start.z; }

    /// Set start momentum
    /// \param px px
    /// \param py py
    /// \param pz pz
    void setStartPxPyPz(Float_t px, Float_t py, Float_t pz) { start.px = px; start.py = py; start.pz = pz; }
    /// Get start momentum x
    /// \return px
    Float_t getStartPx() const { return start.px; }
    /// Get start momentum y
    /// \return py
    Float_t getStartPy() const { return start.py; }
    /// Get start momentum z
    /// \return pz
    Float_t getStartPz() const { return start.pz; }

    /// Set start energy
    /// \param e start energy
    void setStartE(Float_t e) { start.E = e; }
    /// Get start energy
    /// \return energy
    Float_t getStartE() const { return start.E; }

    /// Set stop point
    /// \param x x
    /// \param y y
    /// \param z z
    void setStopXYZ(Float_t x, Float_t y, Float_t z) { stop.x = x; stop.y = y; stop.z = z; }
    /// Get stop x
    /// \return x
    Float_t getStopX() const { return stop.x; }
    /// Get stop y
    /// \return y
    Float_t getStopY() const { return stop.y; }
    /// Get stop z
    /// \return z
    Float_t getStopZ() const { return stop.z; }

    /// Set stop momentum
    /// \param px px
    /// \param py py
    /// \param pz pz
    void setStopPxPyPz(Float_t px, Float_t py, Float_t pz) { stop.px = px; stop.py = py; stop.pz = pz; }
    /// Get stop momentum x
    /// \return px
    Float_t getStopPx() const { return stop.px; }
    /// Get stop momentum y
    /// \return py
    Float_t getStopPy() const { return stop.py; }
    /// Get stop momentum z
    /// \return pz
    Float_t getStopPz() const { return stop.pz; }

    /// Set stop energy
    /// \param e stop energy
    void setStopE(Float_t e) { stop.E = e; }
    /// Get stop energy
    /// \return energy
    Float_t getStopE() const { return stop.E; }

    Double_t getDistance(const TVector3 & ref) const;
    Double_t getRange(const TVector3 & ref) const;

    /// Get scattering
    /// \return scatteringp
    Bool_t getScattering() const { return scattering; }
    /// Set scattering
    /// \param b scattering
    void setScattering(Bool_t b) { scattering = b; }

    /// Get processes
    /// \return processes
    std::vector<std::string> getProcesses() const { return processes; }
    /// Set processes
    /// \param v vector of processes
    void setProcesses(const std::vector<std::string> & v) { processes = v; }

    /// Get Geant4 ID
    /// \return Geant4 ID
    Long_t getG4Id() const { return g4Id; }
    /// Set Geant4 ID
    /// \param i Geant4 ID
    void setG4Id(Long_t i) { g4Id = i; }

    /// Get is stop in the detector
    /// \return success
    Bool_t getStopInDetector() const { return stopInDetector; }
    /// Set is stop in the detector
    /// \param b is stop
    void setStopInDetector(Bool_t b) { stopInDetector = b; }

    /// Get track ID
    /// \return track ID
    Int_t getTrackID() const { return trackID; }
    /// Set track ID
    /// \param i track ID
    void setTrackID(Int_t i) { trackID = i; }
    /// Get parent ID
    /// \return parent ID
    Int_t getParentID() const { return parentID; }
    /// Set parent ID
    /// \param i parent ID
    void setParentID(Int_t i) { parentID = i; }

    /// Get generation process
    /// \return generation process
    std::string getGenerationProcess() const { return generationProcess; }
    /// Set generation process
    /// \param p generation process
    void setGenerationProcess(const std::string & p) { generationProcess = p; }

    void print() const;

    /// Check if pi minus
    /// \return success
    Bool_t isPim() const { return g4Id == -211; }
    /// Check if pi plus
    /// \return success
    Bool_t isPip() const { return g4Id == 211; }
    /// Check if pi zero
    /// \return success
    Bool_t isPiz() const { return g4Id == 111; }
    /// Check if proton
    /// \return success
    Bool_t isP() const { return g4Id == 2212; }
    /// Check if anti-proton
    /// \return success
    Bool_t isaP() const { return g4Id == -2212; }

    /// Get is in acceptance
    /// \return success
    Bool_t getInAcceptance() const { return inAcceptance; }
    /// Set is in acceptance
    /// \param b in acceptance
    void setInAcceptance(Bool_t b) { inAcceptance = b; }

    /// Set active process
    /// \param p process
    void setProcess(Process p) { proc_arr[p] = kTRUE; }
    /// Test process
    /// \param p process
    /// \return success
    Bool_t testProcess(Process p) const { return proc_arr[p]; }
    /// Test process Compton
    /// \return success
    Bool_t isProcessCompton() const { return proc_arr[COMPTON]; }
    /// Test process Inelastic
    /// \return success
    Bool_t isProcessInelastic() const { return proc_arr[INELASTIC]; }
    /// Test process At Rest
    /// \return success
    Bool_t isProcessAtRest() const { return proc_arr[ATREST]; }

    void Clear(Option_t * opt = "");

private:
    ClassDef(SGeantTrack, 1);
};

#endif /* SGEANTTRACK_H */
