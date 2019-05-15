#ifndef TESTGEOMETRY_CALORIMETERSD_H
#define TESTGEOMETRY_CALORIMETERSD_H

#include "CalorimeterHit.h"

#include "G4VSensitiveDetector.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

// FASTSIM
#include "G4VGFlashSensitiveDetector.hh"
class G4GFlashSpot;
class ExGflashDetectorConstruction;
// FASTSIM

/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the hit for the calorimeter.
 *  Based on B5HadCalorimeterSD from examples/basic/B5.
 *
 *  @author Anna Zaborowska
 */
namespace test {
  class CalorimeterSD : public G4VSensitiveDetector, public G4VGFlashSensitiveDetector
{
public:
  CalorimeterSD(G4String name);
  CalorimeterSD(G4String name,G4int aCellNoInAxis);
  CalorimeterSD(G4String name,G4int aCellNoInAxis, G4ThreeVector, G4ThreeVector);
  virtual ~CalorimeterSD();
  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual void EndOfEvent(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
// fastsim
  virtual G4bool ProcessHits(G4GFlashSpot*aSpot,G4TouchableHistory*);
private:
  CalorimeterHitsCollection* fHitsCollection;
  G4int fHCID;
  G4int fCellNo;
  G4ThreeVector fDetectorOffset;
  G4ThreeVector fCellSize;
};
}

#endif /* TESTGEOMETRY_CALORIMETERSD_H */
