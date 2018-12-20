#ifndef TESTGEOMETRY_CALORIMETERSD_H
#define TESTGEOMETRY_CALORIMETERSD_H

#include "CalorimeterHit.h"
#include "TFile.h"
#include "TTree.h"

#include "G4VSensitiveDetector.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the hit for the calorimeter.
 *  Based on B5HadCalorimeterSD from examples/basic/B5.
 *
 *  @author Anna Zaborowska
 */
namespace test {
class CalorimeterSD : public G4VSensitiveDetector
{
public:
  CalorimeterSD(G4String name);
  CalorimeterSD(G4String name,G4int aCellNoInAxis);
  virtual ~CalorimeterSD();
  virtual void Initialize(G4HCofThisEvent*HCE);  
  virtual void EndOfEvent(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
private:
  CalorimeterHitsCollection* fHitsCollection;
  G4int fHCID;
  G4int fCellNo;
  TFile* f;
  TTree* t2;
  CalorimeterHit temp_hit;
  double primary_energy;
  int hitId;
  int event_id;
  G4double pointOfEntryX, pointOfEntryY, pointOfEntryZ;
};
}

#endif /* TESTGEOMETRY_CALORIMETERSD_H */
