//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1SteppingAction.hh 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file B1SteppingAction.hh
/// \brief Definition of the B1SteppingAction class

#ifndef B1SteppingAction_h
#define B1SteppingAction_h 1

// geant4
#include "G4UserSteppingAction.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"

#include "B1DetectorConstruction.hh"

// shared libraries Event.so
class B1DetectorResponse;
class B1Particle;
class DataManager;
class EventSim;

// geometry shared library geometry.so
#include <AbsPart.h>
#include <CADFiber.h>


class B1EventAction;

class G4LogicalVolume;

/// Stepping action class
///

class B1SteppingAction : public G4UserSteppingAction
{
  public:
    B1SteppingAction(B1EventAction* eventAction, DataManager* root, B1DetectorConstruction* det,double kB_);
    virtual ~B1SteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:
    B1EventAction*  fEventAction;
    DataManager* data_manager;
    EventSim* current_event;
    B1Particle* current_particle;
    B1DetectorConstruction* detector_construction;
    B1DetectorResponse* detector_response;

    double kB;
};

G4double calculate_quenched_energy(const G4Step*, G4double, G4double);

#endif