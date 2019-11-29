#include "CalorimeterHit.h"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

namespace test {
G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

CalorimeterHit::CalorimeterHit()
  : G4VHit(), fRhoID(-1), fPhiID(-1), fzID(-1), fEdep(0.), fPos(0) {}

CalorimeterHit::CalorimeterHit(G4int iRho,G4int iPhi,G4int iZ)
: G4VHit(), fRhoID(iRho), fPhiID(iPhi), fzID(iZ), fEdep(0.), fPos(0) {}

CalorimeterHit::~CalorimeterHit() {}

CalorimeterHit::CalorimeterHit(const CalorimeterHit &right) : G4VHit() {
    fRhoID = right.fRhoID;
    fPhiID = right.fPhiID;
    fzID = right.fzID;
    fEdep = right.fEdep;
    fPos = right.fPos;
    fRot = right.fRot;
}

const CalorimeterHit& CalorimeterHit::operator=(const CalorimeterHit &right) {
    fRhoID = right.fRhoID;
    fPhiID = right.fPhiID;
    fzID = right.fzID;
    fEdep = right.fEdep;
    fPos = right.fPos;
    fRot = right.fRot;
    return *this;
}

int CalorimeterHit::operator==(const CalorimeterHit &right) const {
    return (fRhoID==right.fRhoID&&fPhiID==right.fPhiID&&fzID==right.fzID);
}

void CalorimeterHit::Draw() {
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager&&(fEdep>0.))
    {
      G4Transform3D trans(fRot,fPos);
        G4VisAttributes attribs;
        G4Colour colour(fEdep, 0.,0.);
        attribs.SetColour(colour);
        attribs.SetForceSolid(true);
        G4Box box("dummy",1*mm*fEdep/GeV,1*mm*fEdep/GeV,1*mm*fEdep/GeV);
        pVVisManager->Draw(box,attribs,trans);
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
      ->push_back(G4AttValue("Rho",G4UIcommand::ConvertToString(fRhoID), ""));
    values
      ->push_back(G4AttValue("Phi",G4UIcommand::ConvertToString(fPhiID), ""));
    values
      ->push_back(G4AttValue("Z",G4UIcommand::ConvertToString(fzID), ""));
    values
      ->push_back(G4AttValue("Energy",G4BestUnit(fEdep,"Energy"),""));
    values
      ->push_back(G4AttValue("Pos",G4BestUnit(fPos,"Length"),""));
    return values;
}

void CalorimeterHit::Print() {
    //G4cout << "  Cell[" << fxID << ", " << fyID << ", " << fzID << "] "
          // << fEdep/MeV << " (MeV) " << fPos/cm << " cm"<<G4endl;
           std::cout << "  Cell[" << fRhoID << ", " << fPhiID << ", " << fzID << "] "
           << fEdep/MeV << " (MeV) " << fPos/cm << " cm"<< std::endl;
}
}
