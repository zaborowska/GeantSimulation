#include "SaveToFileEventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include "EventInformation.hh"
#include "CalorimeterHit.h"
#include "Analysis.hh"


SaveToFileEventAction::SaveToFileEventAction()
: G4UserEventAction(),
  fHID(-1),
  fCellNoXY(25),
  fCellNoZ(25),
  fCalEdep{ std::vector<G4double>(25*25*25, 0.)},
  fCalX{ std::vector<G4int>(25*25*25, 0)},
  fCalY{ std::vector<G4int>(25*25*25, 0)},
  fCalZ{ std::vector<G4int>(25*25*25, 0)},
  fGflashParams{ std::vector<G4double>(10, 0.)},
  fTimer()
{
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

SaveToFileEventAction::SaveToFileEventAction(G4int aCellNo)
  : G4UserEventAction(),
    fHID(-1),
    fCellNoXY(aCellNo),
    fCellNoZ(aCellNo),
    fCalEdep{ std::vector<G4double>(aCellNo*aCellNo*aCellNo, 0.)},
  fCalX{ std::vector<G4int>(aCellNo*aCellNo*aCellNo, 0)},
  fCalY{ std::vector<G4int>(aCellNo*aCellNo*aCellNo, 0)},
  fCalZ{ std::vector<G4int>(aCellNo*aCellNo*aCellNo, 0)},
  fTimer()
{
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}
SaveToFileEventAction::SaveToFileEventAction(G4int aCellNoXY, G4int aCellNoZ)
  : G4UserEventAction(),
    fHID(-1),
    fCellNoXY(aCellNoXY),
    fCellNoZ(aCellNoZ),
    fCalEdep{ std::vector<G4double>(aCellNoXY*aCellNoXY*aCellNoZ, 0.)},
  fCalX{ std::vector<G4int>(aCellNoXY*aCellNoXY*aCellNoZ, 0)},
  fCalY{ std::vector<G4int>(aCellNoXY*aCellNoXY*aCellNoZ, 0)},
  fCalZ{ std::vector<G4int>(aCellNoXY*aCellNoXY*aCellNoZ, 0)},
  fGflashParams{ std::vector<G4double>(10, 0.)},
  fTimer()
{
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

    fCalEdep.resize(fCellNoXY*fCellNoXY*fCellNoZ);
    fCalX.resize(fCellNoXY*fCellNoXY*fCellNoZ);
    fCalY.resize(fCellNoXY*fCellNoXY*fCellNoZ);
    fCalZ.resize(fCellNoXY*fCellNoXY*fCellNoZ);
    for (G4int ix=0;ix<fCellNoXY;ix++)
      for (G4int iy=0;iy<fCellNoXY;iy++)
        for (G4int iz=0;iz<fCellNoZ;iz++) {
            hitId = fCellNoXY*fCellNoZ*ix+fCellNoZ*iy+iz;
            test::CalorimeterHit* hit = (*hcHC)[hitId];
            G4double eDep = hit->GetEdep();
            if (eDep > 0.0005) { // e > 0.5 keV
                fCalEdep[numNonZeroThresholdCells] = eDep;
                fCalX[numNonZeroThresholdCells] = ix;
                fCalY[numNonZeroThresholdCells] = iy;
                fCalZ[numNonZeroThresholdCells] = iz;
                numNonZeroThresholdCells++;
            }
          }
    fCalEdep.resize(numNonZeroThresholdCells);
    fCalX.resize(numNonZeroThresholdCells);
    fCalY.resize(numNonZeroThresholdCells);
    fCalZ.resize(numNonZeroThresholdCells);

    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, primary_energy);

    EventInformation* eventInformation = dynamic_cast<EventInformation*>(event->GetUserInformation());
    EventInformation::eSimType simtype = eventInformation->GetSimType();
    man->FillNtupleIColumn(5, simtype);
    man->FillNtupleDColumn(6, fTimer.GetRealElapsed());

    man->AddNtupleRow();
}
