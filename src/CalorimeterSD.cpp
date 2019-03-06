#include "CalorimeterSD.h"
#include "CalorimeterHit.h"


#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TROOT.h"
#include <sstream>

namespace test {
CalorimeterSD::CalorimeterSD(G4String name): G4VSensitiveDetector(name),
                                             fHitsCollection(0),
                                             fHCID(-1),
                                             fCellNo(1) {
  collectionName.insert("ECalorimeterColl");

  // t2 = new TTree("t","a Tree with data from an example ");

  // t2->Branch("x", &temp_hit.fxID,"fxID/I");
  // t2->Branch("y", &temp_hit.fyID,"fyID/I");
  // t2->Branch("z", &temp_hit.fzID,"fzID/I");
  // t2->Branch("eDep", &temp_hit.fEdep,"fEdep/D");
  // t2->Branch("primaryE", &primary_energy, "primary_energy/D");
  // t2->Branch("hitId", &hitId, "hitId/I");
}

CalorimeterSD::CalorimeterSD(G4String name, G4int aCellNoInAxis): G4VSensitiveDetector(name),
                                                                  fHitsCollection(0),
                                                                  fHCID(-1),
                                                                  fCellNo(aCellNoInAxis) {
 
  collectionName.insert("ECalorimeterColl");

  // t2 = new TTree("t","a Tree with data from an example");
  
  // t2->Branch("x", &temp_hit.fxID,"fxID/I");
  // t2->Branch("y", &temp_hit.fyID,"fyID/I");
  // t2->Branch("z", &temp_hit.fzID,"fzID/I");
  // t2->Branch("eDep", &temp_hit.fEdep,"fEdep/D");
  // t2->Branch("primaryE", &primary_energy, "primary_energy/D");
  // t2->Branch("hitId", &hitId, "hitId/I");

}

CalorimeterSD::~CalorimeterSD() {
}

void CalorimeterSD::Initialize(G4HCofThisEvent* hce) {

  fHitsCollection = new CalorimeterHitsCollection(SensitiveDetectorName,collectionName[0]);
  if (fHCID<0) {
    fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
  }
  hce->AddHitsCollection(fHCID,fHitsCollection);

  // fill calorimeter hits with zero energy deposition
  for (G4int ix=0;ix<fCellNo;ix++)
    for (G4int iy=0;iy<fCellNo;iy++)
      for (G4int iz=0;iz<fCellNo;iz++)
      {
        CalorimeterHit* hit = new CalorimeterHit();
        fHitsCollection->insert(hit);
      }
     
}

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep==0.) return true;

  G4TouchableHistory* touchable = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  G4int yNo = touchable->GetCopyNumber(1);
  G4int xNo = touchable->GetCopyNumber(2);
  G4int zNo = touchable->GetCopyNumber(0);

  G4int hitID = fCellNo*fCellNo*xNo+fCellNo*yNo+zNo;
  CalorimeterHit* hit = (*fHitsCollection)[hitID];

  if(hit->GetXid()<0)
  {
    hit->SetXid(xNo);
  
    hit->SetYid(yNo);
    
    hit->SetZid(zNo);
    
    G4int depth = touchable->GetHistory()->GetDepth();
    G4AffineTransform transform = touchable->GetHistory()->GetTransform(depth);
    transform.Invert();

    hit->SetRot(transform.NetRotation()); 
    hit->SetPos(transform.NetTranslation());
  
  }
  hit->AddEdep(edep);
  
  //std::cout << "Hit: energy " << edep << " x " << xNo << " y " << yNo << " z " << zNo << std::endl; 
  return true;
}


void CalorimeterSD::EndOfEvent(G4HCofThisEvent* hce) {}
}
