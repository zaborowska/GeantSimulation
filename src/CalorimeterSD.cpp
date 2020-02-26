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

// fastsim

#include "G4GFlashSpot.hh"
#include "EventInformation.hh"
#include "G4EventManager.hh"


namespace test {
CalorimeterSD::CalorimeterSD(G4String name): G4VSensitiveDetector(name),
                                             G4VGFlashSensitiveDetector(),
                                             fHitsCollection(0),
                                             fHCID(-1),
                                             fDebugHCID(-1),
                                             fCellNoRho(1),
                                             fCellNoPhi(1),
                                             fCellNoZ(1),
  frhoDetectorTouchableDepth(2),
  fphiDetectorTouchableDepth(1),
  fzDetectorTouchableDepth(3){
  collectionName.insert("ECalorimeterColl");
  collectionName.insert("debugKilled");
}

CalorimeterSD::CalorimeterSD(G4String name, G4int aCellNoInAxisRho, G4int aCellNoInAxisPhi, G4int aCellNoInAxisZ): G4VSensitiveDetector(name),
                                                                                          G4VGFlashSensitiveDetector(),
                                                                                          fHitsCollection(0),
                                                                                          fHCID(-1),
                                                                                          fDebugHCID(-1),
                                                                                          fCellNoRho(aCellNoInAxisRho),
                                                                                          fCellNoPhi(aCellNoInAxisPhi),
  fCellNoZ(aCellNoInAxisZ),
  frhoDetectorTouchableDepth(2),
  fphiDetectorTouchableDepth(1),
  fzDetectorTouchableDepth(3) {

  collectionName.insert("ECalorimeterColl");
  collectionName.insert("debugKilled");
}
CalorimeterSD::CalorimeterSD(G4String name, G4int aCellNoInAxisRho, G4int aCellNoInAxisPhi, G4int aCellNoInAxisZ,
                             G4int aXdetector, G4int aYdetector, G4int aZdetector ):
  G4VSensitiveDetector(name),
  G4VGFlashSensitiveDetector(),
  fHitsCollection(0),
  fHCID(-1),
  fDebugHCID(-1),
  fCellNoRho(aCellNoInAxisRho),
  fCellNoPhi(aCellNoInAxisPhi),
  fCellNoZ(aCellNoInAxisZ),
  frhoDetectorTouchableDepth(aXdetector),
  fphiDetectorTouchableDepth(aYdetector),
  fzDetectorTouchableDepth(aZdetector)  {

  collectionName.insert("ECalorimeterColl");
  collectionName.insert("debugKilled");
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
  for (G4int iphi=0;iphi<fCellNoPhi;iphi++)
    for (G4int irho=0;irho<fCellNoRho;irho++)
      for (G4int iz=0;iz<fCellNoZ;iz++) {
        CalorimeterHit* hit = new CalorimeterHit();
        fHitsCollection->insert(hit);
      }

  fDebugHitsCollection = new CalorimeterHitsCollection(SensitiveDetectorName,collectionName[1]);
  if (fDebugHCID<0) {
    fDebugHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fDebugHitsCollection);
  }
  hce->AddHitsCollection(fDebugHCID,fDebugHitsCollection);
}

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep==0.) return true;

  G4TouchableHistory* touchable = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  G4int rhoNo = touchable->GetCopyNumber(frhoDetectorTouchableDepth); // cell
  G4int phiNo = touchable->GetCopyNumber(fphiDetectorTouchableDepth); // row
  // G4int materialNo = touchable->GetCopyNumber(0); // which absorber within cell
  G4int zNo = touchable->GetCopyNumber(fzDetectorTouchableDepth); // layer
  // G4int envelopeNo = touchable->GetCopyNumber(4); // calorimeter envelope
  // G4int worldNo = touchable->GetCopyNumber(5); // world volume

  G4int hitID = fCellNoRho*fCellNoZ*phiNo+fCellNoZ*rhoNo+zNo;
  CalorimeterHit* hit = (*fHitsCollection)[hitID];

  if(hit->GetRhoId()<0)
  {
    hit->SetRhoId(rhoNo);
    hit->SetPhiId(phiNo);
    hit->SetZid(zNo);
#ifdef RICHVIS
    hit->SetLogV(touchable->GetVolume(frhoDetectorTouchableDepth)->GetLogicalVolume());
    G4int depth = touchable->GetHistory()->GetDepth();
    G4AffineTransform transform = touchable->GetHistory()->GetTopTransform();
    hit->SetRot(transform.NetRotation());
    transform.Invert();
    hit->SetPos(transform.NetTranslation());
    hit->SetTime(step->GetTrack()->GetGlobalTime());
#endif
  }
  hit->AddEdep(edep);

  if (step->GetPostStepPoint()->GetProcessDefinedStep() != nullptr) {
    if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName().compareTo("debugKill") == 0) {
      CalorimeterHit* debughit = new CalorimeterHit(*hit);
#ifdef RICHVIS
      debughit->SetColour(1);
#endif
      fDebugHitsCollection->insert(debughit);
    }
  }
  return true;
}

// Separate GFLASH interface
G4bool CalorimeterSD::ProcessHits(G4GFlashSpot*aSpot ,G4TouchableHistory*) {
  EventInformation* eventInformation = dynamic_cast<EventInformation*>(G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->GetUserInformation());
  eventInformation->SetSimType(EventInformation::eSimType::eGflash);

  G4double edep = aSpot->GetEnergySpot()->GetEnergy();
  if (edep==0.) return true;
  auto  touchable = aSpot->GetTouchableHandle()->GetHistory();
  G4int rhoNo  = touchable->GetReplicaNo(4); // cell
  G4int phiNo = touchable->GetReplicaNo(3); // row
  G4int zNo = touchable->GetReplicaNo(2); // layer
  // ID = 1 == calorimeter
  // ID 0 == sth else

  G4int hitID = fCellNoRho*fCellNoZ*phiNo+fCellNoZ*rhoNo+zNo;
  CalorimeterHit* hit = (*fHitsCollection)[hitID];
  if(hit->GetRhoId()<0)
  {
    hit->SetRhoId(rhoNo);
    hit->SetPhiId(phiNo);
    hit->SetZid(zNo);
  }
  hit->AddEdep(edep);

  return true;
}

void CalorimeterSD::EndOfEvent(G4HCofThisEvent* hce) {}
}
