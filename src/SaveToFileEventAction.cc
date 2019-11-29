#include "SaveToFileEventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "DetectorConstruction.hh"

#include "EventInformation.hh"
#include "CalorimeterHit.h"
#include "Analysis.hh"


SaveToFileEventAction::SaveToFileEventAction()
: G4UserEventAction(),
  fDetector(nullptr),
  fHID(-1),
  fCellNoRho(25),
  fCellNoPhi(25),
  fCellNoZ(25),
  fCalEdep{ std::vector<G4double>(25*25*25, 0.)},
  fCalRho{ std::vector<G4int>(25*25*25, 0)},
  fCalPhi{ std::vector<G4int>(25*25*25, 0)},
  fCalZ{ std::vector<G4int>(25*25*25, 0)},
  fGflashParams{ std::vector<G4double>(10, 0.)},
  fTimer()
{
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

SaveToFileEventAction::SaveToFileEventAction(G4int aCellNoRho, G4int aCellNoPhi, G4int aCellNoZ)
  : G4UserEventAction(),
    fDetector(nullptr),
    fHID(-1),
    fCellNoRho(aCellNoRho),
    fCellNoPhi(aCellNoPhi),
    fCellNoZ(aCellNoZ),
    fCalEdep{ std::vector<G4double>(aCellNoRho*aCellNoPhi*aCellNoZ, 0.)},
  fCalRho{ std::vector<G4int>(aCellNoRho*aCellNoPhi*aCellNoZ, 0)},
  fCalPhi{ std::vector<G4int>(aCellNoRho*aCellNoPhi*aCellNoZ, 0)},
  fCalZ{ std::vector<G4int>(aCellNoRho*aCellNoPhi*aCellNoZ, 0)},
  fGflashParams{ std::vector<G4double>(10, 0.)},
  fTimer()
{
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}
SaveToFileEventAction::SaveToFileEventAction(const DetectorConstruction* aDetector)
  : G4UserEventAction(),
    fDetector(aDetector),
    fHID(-1),
  fGflashParams{ std::vector<G4double>(10, 0.)},
  fTimer()
{
  fCellNoRho = aDetector->GetNbOfRhoCells();
  fCellNoPhi = aDetector->GetNbOfPhiCells();
  fCellNoZ = aDetector->GetNbOfLayers();
  fCalEdep.reserve(fCellNoRho*fCellNoPhi*fCellNoZ);
  fCalRho.reserve(fCellNoRho*fCellNoPhi*fCellNoZ);
  fCalPhi.reserve(fCellNoRho*fCellNoPhi*fCellNoZ);
  fCalZ.reserve(fCellNoRho*fCellNoPhi*fCellNoZ);
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

void SaveToFileEventAction::BeginOfEventAction(const G4Event* event) {
  // Get hits collections
  if (fHID==-1) {
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce)
      {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl;
        G4Exception("SaveToFileEventAction::EndOfEventAction()",
                    "B5Code001", JustWarning, msg);
        return;
      }
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    fHID = sdManager->GetCollectionID("ECalorimeterColl");
  }
  //New event, add the user information object
  G4EventManager::GetEventManager()->SetUserInformation(new EventInformation);
  fTimer.Start();
}

void SaveToFileEventAction::EndOfEventAction(const G4Event* event)
{
  fTimer.Stop();
  G4HCofThisEvent* hce = event->GetHCofThisEvent();
  test::CalorimeterHitsCollection* hcHC
    = static_cast<test::CalorimeterHitsCollection*>(hce->GetHC(fHID));

  if ( (!hcHC) )
    {
      G4ExceptionDescription msg;
      msg << "Some of hits collections of this event not found." << G4endl;
      G4Exception("SaveToFileEventAction::EndOfEventAction()",
                  "B5Code001", JustWarning, msg);
      return;
    }
    double primary_energy;
    int hitId;
    primary_energy = G4EventManager::GetEventManager()->GetConstCurrentEvent()
      ->GetPrimaryVertex()->GetPrimary(0)->GetTotalEnergy();
    G4int numNonZeroThresholdCells = 0;

    fCalEdep.resize(fCellNoRho*fCellNoPhi*fCellNoZ);
    fCalRho.resize(fCellNoRho*fCellNoPhi*fCellNoZ);
    fCalPhi.resize(fCellNoRho*fCellNoPhi*fCellNoZ);
    fCalZ.resize(fCellNoRho*fCellNoPhi*fCellNoZ);
    for (G4int iphi=0;iphi<fCellNoPhi;iphi++)
      for (G4int irho=0;irho<fCellNoRho;irho++)
        for (G4int iz=0;iz<fCellNoZ;iz++) {
            hitId = fCellNoRho*fCellNoZ*iphi+fCellNoZ*irho+iz;
            test::CalorimeterHit* hit = (*hcHC)[hitId];
            G4double eDep = hit->GetEdep();
            if (eDep > 0.0005) { // e > 0.5 keV
                fCalEdep[numNonZeroThresholdCells] = eDep;
                fCalRho[numNonZeroThresholdCells] = irho;
                fCalPhi[numNonZeroThresholdCells] = iphi;
                fCalZ[numNonZeroThresholdCells] = iz;
                numNonZeroThresholdCells++;
            }
          }
    fCalEdep.resize(numNonZeroThresholdCells);
    fCalRho.resize(numNonZeroThresholdCells);
    fCalPhi.resize(numNonZeroThresholdCells);
    fCalZ.resize(numNonZeroThresholdCells);

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, primary_energy);

    EventInformation* eventInformation = dynamic_cast<EventInformation*>(event->GetUserInformation());
    EventInformation::eSimType simtype = eventInformation->GetSimType();
    man->FillNtupleIColumn(5, simtype);
    man->FillNtupleDColumn(6, fTimer.GetRealElapsed());
    fGflashParams = eventInformation->GetGflashParams();

    man->AddNtupleRow();
}
void SaveToFileEventAction::UpdateParameters() {
  if (fDetector == nullptr) {
    return;
  }
  fCellNoRho = fDetector->GetNbOfRhoCells();
  fCellNoPhi = fDetector->GetNbOfPhiCells();
  fCellNoZ = fDetector->GetNbOfLayers();
}
