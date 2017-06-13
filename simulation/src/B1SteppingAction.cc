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
// $Id: B1SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalPhoton.hh"
#include "G4EmCalculator.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(B1EventAction* eventAction, DataManager* root, B1DetectorConstruction* det, double kB_)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{
  data_manager = root;
  detector_construction  = det;
  kB = kB_;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
  // get volume of the current step
  G4LogicalVolume* volume
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();

  // get the DetectorPart according to the current step logical volume
  AbsPart* part = detector_construction->getGeo()->getPart(volume);

  // Is there a detector part?
  if (part == NULL)
    return;

  // get the current_particle and the current event
  current_event = data_manager->getEvent()->getSimulatedEvent();
  int track_ID = step->GetTrack()->GetTrackID();
  current_particle = current_event->getParticle(track_ID);

  // get the step defining process and add it to the process list of the current particle if the process is important
  // important processes are defined in the else if section
  const G4String process = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  if (process == "compt")
    {
      current_particle->Add_Process(process);
    }
  else if(process == "anti_protonInelastic") // empty example -> insert another important process name here
    {
      current_particle->Add_Process(process);
    }
  else if(process == "hFritiofCaptureAtRest") // empty example -> insert another important process name here
    {
      current_particle->Add_Process(process);
    }

  // stop in detektor volume
  if (part != NULL)
    {
      // set flag
      current_particle->Set_stop_in_detector(true);
    }
  else
    {
      // particle did not stop in detektor
      current_particle->Set_stop_in_detector(false);
    }


  // Energy Deposit in fiber
  // is the current volume a fiber?
  CADFiber* fiber;
  if (fiber = dynamic_cast<CADFiber*> (part))
    {
      detector_response = current_event->getDetectorResponse();
      int x_fiber = fiber->getFiberX();
      int y_fiber = fiber->getFiberY();
      G4double energy_step = step->GetTotalEnergyDeposit() - step->GetNonIonizingEnergyDeposit (); //take here only ionization energy loss
      // formular for  quenching
      double energy_step_quenching =  calculate_quenched_energy(step, kB, energy_step);

	      if (detector_response->Get_energy(x_fiber, y_fiber) == 0 && energy_step > 0)
			{
				detector_response->Add_fiber_hit();
			}

      detector_response->Set_energy (x_fiber,y_fiber, energy_step);
      detector_response->Set_energy_quenching (x_fiber,y_fiber, energy_step_quenching);
      detector_response->Set_total_energy (energy_step);
      }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4double calculate_quenched_energy(const G4Step* step, G4double kB, G4double E_dep){
	G4double E_quenched = 0;
	G4double E_before = step->GetPreStepPoint()->GetKineticEnergy();
	G4double E_after = step->GetPostStepPoint()->GetKineticEnergy();
	G4EmCalculator *EmCalculator = new G4EmCalculator();
	G4ParticleDefinition* particle = step->GetTrack()->GetDefinition();
	G4Material* material = step->GetPreStepPoint()->GetMaterial();

	G4double dEdx = EmCalculator->GetDEDX(E_before, particle, material, 0);
	E_quenched += E_dep / (1 + kB * dEdx);


	//G4double range_step = range_diff / (double) substeps;


	//for (int n = 0; n < substeps; n++){
	//	G4double E1 = 0;
	//	G4double E2 = 0;
	//	if (n == 0){
	//		E1 = E_before;
	//	}
	//	else {
	//		E1 = EmCalculator->GetKinEnergy(total_range - range_step * n, particle, material,0);
	//	}
	//
	//	//E2 = EmCalculator->GetKinEnergy(total_range - range_step * (n + 1), particle, material, 0);
	//	E2 = E1 - EmCalculator->GetDEDX(E1,particle,material,0) * range_step;
	//
	//	G4double dEdx = EmCalculator->GetDEDX(E1, particle, material, 0);
	//	if (E1 >= E2){
	//		E_quenched += (E1 - E2)  / (1 + kB * dEdx);
	//		//printf("E1_corr = %e \n", E1);
	//		//printf("Range_1 = %e \n",total_range - range_step * n);
	//		//printf("E2_corr = %e \n", E2);
	//		//printf("Range_2 = %e \n",total_range - range_step * (n + 1));
	//	}
	//	else {
	//		//printf("E1 = %e \n", E1);
	//		//printf("Range_1 = %e \n",total_range - range_step * n);
	//		//printf("E2 = %e \n", E2);
	//		//printf("Range_2 = %e \n",total_range - range_step * (n + 1));
    //
	//	}
	//}

	delete EmCalculator;
	return E_quenched;

}
