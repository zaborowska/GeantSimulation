#include "ParallelWorldForReadout.h"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4PVReplica.hh"
#include "G4Box.hh"

#include "DetectorConstruction.hh"
#include "CalorimeterSD.h"

#include "G4SDManager.hh"

// FASTSIM
#include "myGFlashHomoShowerParameterisation.hh"
#include "GFlashSamplingShowerParameterisation.hh"
#include "G4FastSimulationManager.hh"
#include "myGFlashShowerModel.hh"
#include "myGFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
// FASTSIM

ParallelWorldForReadout::ParallelWorldForReadout(G4String aWorldName, const DetectorConstruction* aMassDetector) :
 G4VUserParallelWorld(aWorldName), massDetector(aMassDetector) {}
ParallelWorldForReadout::~ParallelWorldForReadout() {}

void ParallelWorldForReadout::Construct()
{
  // -- Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();
  // Build materials
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
//  Build parallel/ghost geometry:
  G4VPhysicalVolume* ghostWorld = GetWorld();
  auto ghostLogicalVolume = ghostWorld->GetLogicalVolume();
  std::cout << "ghost world size " << dynamic_cast<G4Box*>(ghostLogicalVolume->GetSolid())->GetXHalfLength () << " x "
            << dynamic_cast<G4Box*>(ghostLogicalVolume->GetSolid())->GetYHalfLength () << " x "
            << dynamic_cast<G4Box*>(ghostLogicalVolume->GetSolid())->GetZHalfLength () << std::endl;

  auto fCalorSizeYZ = massDetector->GetCalorSizeYZ();
  auto fCalorThickness = massDetector->GetCalorThickness();
  auto fNbOfLayers = massDetector->GetNbOfLayers();
  auto fLayerThickness = fCalorThickness / fNbOfLayers;
  auto fNbOfCells = massDetector->GetNbOfCells();

  auto fSolidCalor = new G4Box("Calorimeter",fCalorSizeYZ/2,fCalorSizeYZ/2,
                       fCalorThickness/2);

  fLogicCalor = new G4LogicalVolume(fSolidCalor,
                                         air,
                                         "Calorimeter");
  // FASTSTIM
  caloRegion = new G4Region("Calorimeter_region_parallelgeo");
  caloRegion->AddRootLogicalVolume(fLogicCalor);//fLogicCalor);
  // FASTSIM

  auto fPhysiCalor = new G4PVPlacement(0,                     //no rotation
                                 G4ThreeVector(0, 0, fCalorThickness/2),        //at (0,0,0)
                                 fLogicCalor,            //its fLogical volume
                                 "Calorimeter",          //its name
                                 ghostLogicalVolume,            //its mother  volume
                                 false,                  //no boolean operation
                                 80);                     //copy number

  //
  // Layers
  //

  auto fSolidLayer = new G4Box("Layer",fCalorSizeYZ/2,fCalorSizeYZ/2,fLayerThickness/2);

  auto fLogicLayer = new G4LogicalVolume(fSolidLayer,
                                   air,
                                   "Layer");
  if (fNbOfLayers > 1)
    auto fPhysiLayer = new G4PVReplica("Layer",
                                  fLogicLayer,
                                  fLogicCalor,
                                  kZAxis,
                                  fNbOfLayers,
                                  fLayerThickness);
  else
    auto fPhysiLayer = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicLayer,
                                   "Layer",
                                   fLogicCalor,
                                   false,
                                   0);

  //
  // Cells along Y = row
  //

  auto fSolidRow = new G4Box("Row",fCalorSizeYZ/2,fCalorSizeYZ/fNbOfCells/2,
                       fLayerThickness/2);

  auto fLogicRow = new G4LogicalVolume(fSolidRow,
                                   air,
                                   "Row");
  if (fNbOfCells > 1)
    auto fPhysiRow = new G4PVReplica("Row",
                                fLogicRow,
                                fLogicLayer,
                                 kYAxis,
                                 fNbOfCells,
                                 fCalorSizeYZ/fNbOfCells);
  else
    auto fPhysiRow = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicRow,
                                   "Row",
                                   fLogicLayer,
                                   false,
                                   0);

  //
  // Cells along Z = single cell
  //

  auto fSolidCell = new G4Box("Cell", fCalorSizeYZ/fNbOfCells/2,fCalorSizeYZ/fNbOfCells/2,
                       fLayerThickness/2);

  fLogicCell = new G4LogicalVolume(fSolidCell,
                                   air,
                                   "Cell");
  if (fNbOfCells > 1)
    auto fPhysiCell = new G4PVReplica("Cell",
                                fLogicCell,
                                fLogicRow,
                                 kXAxis,
                                 fNbOfCells,
                                 fCalorSizeYZ/fNbOfCells);
  else
    auto fPhysiCell = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicCell,
                                   "Cell",
                                   fLogicRow,
                                   false,
                                   0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParallelWorldForReadout::ConstructSD()
{
  auto fCalorSizeYZ = massDetector->GetCalorSizeYZ();
  auto fCalorThickness = massDetector->GetCalorThickness();
  auto fNbOfLayers = massDetector->GetNbOfLayers();
  auto fLayerThickness = fCalorThickness / fNbOfLayers;
  auto fNbOfCells = massDetector->GetNbOfCells();

  std::string caloSDname = "ECal";
  std::cout << " Construct SD " << " num cells = " << fNbOfCells << "   num layers = " << fNbOfLayers <<
    " transverse size = " << fCalorSizeYZ <<  " layer thickness = " <<  fLayerThickness << std::endl;
  test::CalorimeterSD* caloSD = new test::CalorimeterSD(caloSDname,fNbOfCells,fNbOfLayers,
                                                        G4ThreeVector(fCalorSizeYZ/2,fCalorSizeYZ/2,0),
                                                        G4ThreeVector(fCalorSizeYZ/fNbOfCells, fCalorSizeYZ/fNbOfCells, fLayerThickness),
                                                        0,1,2);
  G4SDManager::GetSDMpointer()->AddNewDetector(caloSD);
  SetSensitiveDetector(fLogicCell, caloSD);
  std::cout << " Construct SD " << std::endl;
  // FASTSIM
  // for the moment only for homo calorimeters:
  auto fFastShowerModel = new myGFlashShowerModel("fastShowerModel", caloRegion);
  G4NistManager* nistManager = G4NistManager::Instance();
  if (massDetector->GetNbOfAbsor() == 1) {
    auto fParameterisation = new myGFlashHomoShowerParameterisation(massDetector->GetAbsorMaterial(0));
    fFastShowerModel->SetParameterisation(*fParameterisation);
  } else if (massDetector->GetNbOfAbsor() == 2) {
    auto fParameterisation = new GFlashSamplingShowerParameterisation(massDetector->GetAbsorMaterial(0),
                                                                      massDetector->GetAbsorMaterial(1),
                                                                      massDetector->GetAbsorThickness(0),massDetector->GetAbsorThickness(1));
    fFastShowerModel->SetParameterisation(*fParameterisation);
  } else {
    G4cout << "WARNING! Only two absorbers can be defined in a sampling calorimeter" << G4endl;
  }
  // Energy Cuts to kill particles:
  auto fParticleBounds = new GFlashParticleBounds();
  fFastShowerModel->SetParticleBounds(*fParticleBounds);
  // Makes the EnergieSpots
  auto fHitMaker = new myGFlashHitMaker();
  fHitMaker->SetUseSDOfEnvelopeWorld(true);
  fFastShowerModel->SetHitMaker(*fHitMaker);
  //FASTSIM
}
