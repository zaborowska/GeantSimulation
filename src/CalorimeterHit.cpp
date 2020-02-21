#include "CalorimeterHit.h"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"

namespace test {
G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

CalorimeterHit::CalorimeterHit()
  : G4VHit(), fRhoID(-1), fPhiID(-1), fzID(-1), fEdep(0.), fTime(0.), fPos(0), fLogV(nullptr), fColour(0) {}

CalorimeterHit::CalorimeterHit(G4int iRho,G4int iPhi,G4int iZ)
: G4VHit(), fRhoID(iRho), fPhiID(iPhi), fzID(iZ), fEdep(0.), fTime(0.), fPos(0), fLogV(nullptr), fColour(0) {}

CalorimeterHit::CalorimeterHit(G4int iRho,G4int iPhi,G4int iZ, G4LogicalVolume* aLogV)
: G4VHit(), fRhoID(iRho), fPhiID(iPhi), fzID(iZ), fEdep(0.), fTime(0.), fPos(0), fLogV(aLogV), fColour(0)  {}

CalorimeterHit::~CalorimeterHit() {}

CalorimeterHit::CalorimeterHit(const CalorimeterHit &right) : G4VHit() {
    fRhoID = right.fRhoID;
    fPhiID = right.fPhiID;
    fzID = right.fzID;
    fEdep = right.fEdep;
    fTime = right.fTime;
    fPos = right.fPos;
    fRot = right.fRot;
    fColour = right.fColour;
    fLogV = right.fLogV;
}

const CalorimeterHit& CalorimeterHit::operator=(const CalorimeterHit &right) {
    fRhoID = right.fRhoID;
    fPhiID = right.fPhiID;
    fzID = right.fzID;
    fEdep = right.fEdep;
    fTime = right.fTime;
    fPos = right.fPos;
    fRot = right.fRot;
    fColour = right.fColour;
    fLogV = right.fLogV;
    return *this;
}

int CalorimeterHit::operator==(const CalorimeterHit &right) const {
    return (fRhoID==right.fRhoID&&fPhiID==right.fPhiID&&fzID==right.fzID);
}

void CalorimeterHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(! (fEdep > 0) ) return;
  if (! pVVisManager->FilterHit(*this)) return;
  if (pVVisManager) {
    G4Transform3D trans(fRot,fPos);
    G4VisAttributes attribs;
    G4Tubs solid("dummy", 0, 10*cm, 10*cm, 0, 0.5*CLHEP::pi);
    if(fLogV) {
      const G4VisAttributes* pVA = fLogV->GetVisAttributes();
      if(pVA) attribs = *pVA;
      // cannot use directly fLogV due to rho parametrisation (change of solid!)
      solid = *dynamic_cast<G4Tubs*>(fLogV->GetSolid());
      double dR = solid.GetRMax() - solid.GetRMin();
      solid.SetInnerRadius(solid.GetRMin() + fRhoID * dR);
      solid.SetOuterRadius(solid.GetRMax() + fRhoID * dR);
    }
    G4double r=0, g=0, b=0;
    switch(fColour) {
    case 0:
      r = 1;
      break;
    case 1:
      b = 1;
      break;
    }
    G4Colour colour(r, g, b );
    attribs.SetColour(colour);
    attribs.SetForceSolid(true);
    pVVisManager->Draw(solid,attribs,trans);
  }
}

const std::map<G4String,G4AttDef>* CalorimeterHit::GetAttDefs() const {
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
    = G4AttDefStore::GetInstance("CalorimeterHit",isNew);
    if (isNew) {
        (*store)["HitType"]
          = G4AttDef("HitType","Hit Type","Physics","","G4String");
        (*store)["Rho"]
          = G4AttDef("Rho","rho ID","Physics","","G4int");
        (*store)["Phi"]
          = G4AttDef("Phi","phi ID","Physics","","G4int");
        (*store)["Z"]
          = G4AttDef("Z","z ID","Physics","","G4int");
        (*store)["Energy"]
          = G4AttDef("Energy","Energy Deposited","Physics","G4BestUnit",
                     "G4double");
        (*store)["Time"]
          = G4AttDef("Time","Time","Physics","G4BestUnit",
                     "G4double");
        (*store)["Pos"]
          = G4AttDef("Pos", "Position", "Physics","G4BestUnit",
                     "G4ThreeVector");
    }
    return store;
}

std::vector<G4AttValue>* CalorimeterHit::CreateAttValues() const {
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    values
      ->push_back(G4AttValue("HitType","HadCalorimeterHit",""));
    values
      ->push_back(G4AttValue("Rho",fRhoID, ""));
    values
      ->push_back(G4AttValue("Phi",fPhiID, ""));
    values
      ->push_back(G4AttValue("Z",fzID, ""));
    values
      ->push_back(G4AttValue("Energy",G4BestUnit(fEdep,"Energy"),""));
    values
      ->push_back(G4AttValue("Time",G4BestUnit(fTime,"Time"),""));
    values
      ->push_back(G4AttValue("Pos",G4BestUnit(fPos,"Length"),""));
    return values;
}

void CalorimeterHit::Print() {
  std::cout << "  Cell[" << fRhoID << ", " << fPhiID << ", " << fzID << "] "
            << fEdep/MeV << " (MeV) " << fPos/cm << " cm  " << fTime << " ns"<< std::endl;
}
}
