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
                                             fCellNoXY(1),
                                             fCellNoZ(1),
                                             fDetectorOffset(G4ThreeVector()),
                                             fCellSize(G4ThreeVector()),
  fxDetectorTouchableDepth(2),
  fyDetectorTouchableDepth(1),
  fzDetectorTouchableDepth(3){
  collectionName.insert("ECalorimeterColl");
}

CalorimeterSD::CalorimeterSD(G4String name, G4int aCellNoInAxisXY, G4int aCellNoInAxisZ): G4VSensitiveDetector(name),
                                                                                          G4VGFlashSensitiveDetector(),
                                                                                          fHitsCollection(0),
                                                                                          fHCID(-1),
                                                                                          fCellNoXY(aCellNoInAxisXY),
  fCellNoZ(aCellNoInAxisZ),
  fDetectorOffset(G4ThreeVector()),
  fCellSize(G4ThreeVector()),
  fxDetectorTouchableDepth(2),
  fyDetectorTouchableDepth(1),
  fzDetectorTouchableDepth(3) {

  collectionName.insert("ECalorimeterColl");
}
CalorimeterSD::CalorimeterSD(G4String name, G4int aCellNoInAxisXY, G4int aCellNoInAxisZ, G4ThreeVector aDetectorOffset, G4ThreeVector aCellSize ):
  G4VSensitiveDetector(name),
  G4VGFlashSensitiveDetector(),
  fHitsCollection(0),
  fHCID(-1),
  fCellNoXY(aCellNoInAxisXY),
  fCellNoZ(aCellNoInAxisZ),
  fDetectorOffset(aDetectorOffset),
  fCellSize(aCellSize),
  fxDetectorTouchableDepth(2),
  fyDetectorTouchableDepth(1),
  fzDetectorTouchableDepth(3)  {

  collectionName.insert("ECalorimeterColl");
}
CalorimeterSD::CalorimeterSD(G4String name, G4int aCellNoInAxisXY, G4int aCellNoInAxisZ, G4ThreeVector aDetectorOffset, G4ThreeVector aCellSize,
                             G4int aXdetector, G4int aYdetector, G4int aZdetector ):
  G4VSensitiveDetector(name),
  G4VGFlashSensitiveDetector(),
  fHitsCollection(0),
  fHCID(-1),
  fCellNoXY(aCellNoInAxisXY),
  fCellNoZ(aCellNoInAxisZ),
  fDetectorOffset(aDetectorOffset),
  fCellSize(aCellSize),
  fxDetectorTouchableDepth(aXdetector),
  fyDetectorTouchableDepth(aYdetector),
  fzDetectorTouchableDepth(aZdetector)  {

  collectionName.insert("ECalorimeterColl");
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
  for (G4int ix=0;ix<fCellNoXY;ix++)
    for (G4int iy=0;iy<fCellNoXY;iy++)
      for (G4int iz=0;iz<fCellNoZ;iz++) {
        CalorimeterHit* hit = new CalorimeterHit(fCellSize.x(),fCellSize.z());
        fHitsCollection->insert(hit);
      }
}

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep==0.) return true;

  G4TouchableHistory* touchable = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  G4int yNo = touchable->GetCopyNumber(fyDetectorTouchableDepth); // cell
  G4int xNo = touchable->GetCopyNumber(fxDetectorTouchableDepth); // row
  // G4int materialNo = touchable->GetCopyNumber(0); // which absorber within cell
  G4int zNo = touchable->GetCopyNumber(fzDetectorTouchableDepth); // layer
  // G4int envelopeNo = touchable->GetCopyNumber(4); // calorimeter envelope
  // G4int worldNo = touchable->GetCopyNumber(5); // world volume

  G4int hitID = fCellNoXY*fCellNoZ*xNo+fCellNoZ*yNo+zNo;
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
  // std::cout << "Hit: energy " << edep << " x " << xNo << " y " << yNo << " z " << zNo 
  //           << std::endl;
  return true;
}

// Separate GFLASH interface
G4bool CalorimeterSD::ProcessHits(G4GFlashSpot*aSpot ,G4TouchableHistory*) {
  EventInformation* eventInformation = dynamic_cast<EventInformation*>(G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->GetUserInformation());
  eventInformation->SetSimType(EventInformation::eSimType::eGflash);

  G4double edep = aSpot->GetEnergySpot()->GetEnergy();
  if (edep==0.) return true;
  auto  touchable = aSpot->GetTouchableHandle()->GetHistory();
  G4int yNo  = touchable->GetReplicaNo(3); // cell
  G4int xNo = touchable->GetReplicaNo(4); // row
  G4int zNo = touchable->GetReplicaNo(2); // layer

  G4int hitID = fCellNoXY*fCellNoZ*xNo+fCellNoZ*yNo+zNo;
  CalorimeterHit* hit = (*fHitsCollection)[hitID];
  if(hit->GetXid()<0)
  {
    hit->SetXid(xNo);
    hit->SetYid(yNo);
    hit->SetZid(zNo);
  }
  hit->AddEdep(edep);

  return true;
}

void CalorimeterSD::EndOfEvent(G4HCofThisEvent* hce) {}
}
