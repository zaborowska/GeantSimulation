#include "SaveToFileEventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include "CalorimeterHit.h"
#include "Analysis.hh"


SaveToFileEventAction::SaveToFileEventAction()
: G4UserEventAction(),
  fHID(-1),
  fCellNo(25),
  fCalEdep{ std::vector<G4double>(25*25*25, 0.)},
  fCalX{ std::vector<G4int>(25*25*25, 0)},
  fCalY{ std::vector<G4int>(25*25*25, 0)},
  fCalZ{ std::vector<G4int>(25*25*25, 0)}
{
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
}

SaveToFileEventAction::SaveToFileEventAction(G4int aCellNo)
  : G4UserEventAction(),
    fHID(-1),
    fCellNo(aCellNo),
    fCalEdep{ std::vector<G4double>(aCellNo*aCellNo*aCellNo, 0.)},
  fCalX{ std::vector<G4int>(aCellNo*aCellNo*aCellNo, 0)},
  fCalY{ std::vector<G4int>(aCellNo*aCellNo*aCellNo, 0)},
  fCalZ{ std::vector<G4int>(aCellNo*aCellNo*aCellNo, 0)}
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
}

void SaveToFileEventAction::EndOfEventAction(const G4Event* event)
{
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

    fCalEdep.resize(fCellNo*fCellNo*fCellNo);
    fCalX.resize(fCellNo*fCellNo*fCellNo);
    fCalY.resize(fCellNo*fCellNo*fCellNo);
    fCalZ.resize(fCellNo*fCellNo*fCellNo);
    for (G4int ix=0;ix<fCellNo;ix++)
      for (G4int iy=0;iy<fCellNo;iy++)
        for (G4int iz=0;iz<fCellNo;iz++) {
            hitId = fCellNo*fCellNo*ix+fCellNo*iy+iz;
            test::CalorimeterHit* hit = (*hcHC)[hitId];
            G4double eDep = hit->GetEdep();
            if (eDep > 0.1) {
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
    man->AddNtupleRow();
}
