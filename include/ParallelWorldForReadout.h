#ifndef PARALLELWORLDFORREADOUT_HH
#define PARALLELWORLDFORREADOUT_HH

#include "G4VUserParallelWorld.hh"
class DetectorConstruction;
class G4Region;

class ParallelWorldForReadout : public G4VUserParallelWorld {
public:
ParallelWorldForReadout(G4String aWorldName, const DetectorConstruction* aMassDetector);
  ~ParallelWorldForReadout();

private:
  virtual void Construct();
  virtual void ConstructSD();
  const DetectorConstruction* massDetector;
  G4LogicalVolume*   fLogicCell;
  G4LogicalVolume*   fLogicCalor;
  G4Region* caloRegion;
};

#endif
