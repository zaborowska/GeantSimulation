#include "SaveToFileEventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include "CalorimeterHit.h"


SaveToFileEventAction::SaveToFileEventAction()
: G4UserEventAction(),
  fHID(-1),
  fCellNo(32),
  fOutput(nullptr)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(10);
}

SaveToFileEventAction::SaveToFileEventAction(TFile* aFile, G4int aCellNo)
  : G4UserEventAction(),
    fHID(-1),
    fCellNo(aCellNo),
    fOutput(aFile)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(10);
  std::cout<< " INITIALIZE EVENT ACTION" << std::endl;
}

void SaveToFileEventAction::BeginOfEventAction(const G4Event*) {}

void SaveToFileEventAction::EndOfEventAction(const G4Event* event)
{
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce)
    {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl;
        G4Exception("SaveToFileEventAction::EndOfEventAction()",
                    "B5Code001", JustWarning, msg);
        return;
    }

    // Get hits collections

    if (fHID==-1) {
      G4SDManager* sdManager = G4SDManager::GetSDMpointer();
      fHID = sdManager->GetCollectionID("ECalorimeterColl");
    }
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

    //
    // Fill histograms & ntuple
    //
    test::CalorimeterHit temp_hit;
  double primary_energy;
  int hitId;
  int event_id;
  G4double pointOfEntryX, pointOfEntryY, pointOfEntryZ;

  event_id = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  std::cout << "EVENT ID PRINT:" << event_id << std::endl;

  primary_energy = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary(0)->GetTotalEnergy();
  std::cout << "The primary energy is: "<< primary_energy << std::endl;

  std::string nameOfTTree = "t_";
  std::ostringstream primaryEnergyString1;
  primaryEnergyString1 << primary_energy;
  std::string primaryEnergyString = primaryEnergyString1.str();
  nameOfTTree += primaryEnergyString;
  //nameOfTTree += std::to_string(event_id);

  fOutput->cd();
  t2 = new TTree(nameOfTTree.c_str(),"a Tree with one generated event");
  t2->SetDirectory(fOutput);
  t2->Branch("x", &temp_hit.fxID,"fxID/I");
  t2->Branch("y", &temp_hit.fyID,"fyID/I");
  t2->Branch("z", &temp_hit.fzID,"fzID/I");
  t2->Branch("eDep", &temp_hit.fEdep,"fEdep/D");
  // t2->Branch("primaryE", &primary_energy, "primary_energy/D");
  // t2->Branch("hitId", &hitId, "hitId/I");


  for (G4int ix=0;ix<32;ix++)
    for (G4int iy=0;iy<32;iy++)
      for (G4int iz=0;iz<32;iz++)
        {
          hitId = fCellNo*fCellNo*ix+fCellNo*iy+iz;
          test::CalorimeterHit* hit = (*hcHC)[hitId];
          G4double eDep = hit->GetEdep();
          if (eDep > 0.1) {
            temp_hit.SetXid(ix);
            temp_hit.SetYid(iy);
            temp_hit.SetZid(iz);
            temp_hit.SetEdep(eDep);

            t2->Fill();
          }
        }

  t2->Write();

    // Fill ntuple
    G4int totalHadHit = 0;
    G4double totalHadE = 0.;
    for (G4int i=0;i<fCellNo*fCellNo*fCellNo;i++)
    {
      test::CalorimeterHit* hit = (*hcHC)[i];
      G4double eDep = hit->GetEdep();
      if (eDep>0.)
      {
        totalHadHit++;
        totalHadE += eDep;
      }
      if(hit->GetXid() != -1 && hit->GetYid() != -1 && hit->GetZid() != -1) {
        //hit->Print();
        //G4cout<<"Storing hit "<<eDep<<" "<<fCellNo*fCellNo*hit->GetXid()+fCellNo*hit->GetYid()+hit->GetZid()<<G4endl;

      }
    }
    G4cout<<" __write current event..."<<G4endl;

    // Print diagnostics
    G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( printModulo==0 || event->GetEventID() % printModulo != 0) return;

    // Had calorimeter
    G4cout << "Hadron Calorimeter has " << totalHadHit << " hits. Total Edep is "
           << totalHadE/MeV << " (MeV)" << G4endl;

}
