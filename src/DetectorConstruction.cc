#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4GeometryManager.hh"
#include "G4SDManager.hh"

#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

#include "CalorimeterSD.h"

#include <iomanip>

// FASTSIM
#include "GFlashHomoShowerParameterisation.hh"
#include "G4FastSimulationManager.hh"
#include "GFlashShowerModel.hh"
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
// FASTSIM

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
 :G4VUserDetectorConstruction(),
  fWorldMaterial(nullptr),fSolidWorld(nullptr),fLogicWorld(nullptr),
  fPhysiWorld(nullptr),fSolidCalor(nullptr),fLogicCalor(nullptr),
  fPhysiRow(nullptr),fSolidRow(nullptr),fLogicRow(nullptr),
  fPhysiCell(nullptr),fSolidCell(nullptr),fLogicCell(nullptr),
  fPhysiCalor(nullptr),fSolidLayer(nullptr),fLogicLayer(nullptr),
  fPhysiLayer(nullptr),fDetectorMessenger(nullptr)
{
  fDetectorMessenger= new DetectorMessenger(this);

  // default parameter values of the calorimeter
  fNbOfAbsor = 4;
  fAbsorThickness[0] = 1.9*mm;
  fAbsorThickness[1] = 0.5*mm;
  fAbsorThickness[2] = 1.9*mm;
  fAbsorThickness[3] = 0.5*mm;
  fNbOfLayers        = 25;
  fNbOfCells         = 25;
  fCalorSizeYZ       = 5*mm * fNbOfCells;

  // materials
  SetWorldMaterial("G4_Galactic");
  SetAbsorMaterial(0,"G4_W");
  SetAbsorMaterial(1,"G4_Si");
  SetAbsorMaterial(2,"G4_W");
  SetAbsorMaterial(3,"G4_Si");
  SetAbsorSensitive(0,false);
  SetAbsorSensitive(1,true);
  SetAbsorSensitive(2,false);
  SetAbsorSensitive(3,true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ComputeCalorParameters()
{
  // Compute derived parameters of the calorimeter
  fLayerThickness = 0.;
  for (G4int iAbs=0; iAbs<fNbOfAbsor; iAbs++) {
    fLayerThickness += fAbsorThickness[iAbs];
  }
  fCalorThickness = fNbOfLayers*fLayerThickness;
  fWorldSizeX = 2*fCalorThickness;
  fWorldSizeYZ = 1.2*fCalorSizeYZ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  if(fPhysiWorld) { return fPhysiWorld; }
  ComputeCalorParameters();

  fSolidWorld = new G4Box("World",                                //its name
                   fWorldSizeYZ/2,fWorldSizeYZ/2,fWorldSizeX/2);  //its size

  fLogicWorld = new G4LogicalVolume(fSolidWorld,            //its solid
                                   fWorldMaterial,        //its material
                                   "World");                //its name

  fPhysiWorld = new G4PVPlacement(0,                      //no rotation
                                  G4ThreeVector(),       //at (0,0,0)
                                 fLogicWorld,             //its fLogical volume
                                 "World",                 //its name
                                 0,                       //its mother  volume
                                 false,                   //no boolean operation
                                 99);                      //copy number

  fSolidCalor = new G4Box("Calorimeter",fCalorSizeYZ/2,fCalorSizeYZ/2,
                       fCalorThickness/2);

  fLogicCalor = new G4LogicalVolume(fSolidCalor,
                                         fWorldMaterial,
                                         "Calorimeter");
  // FASTSTIM
  G4Region* caloRegion = new G4Region("Calorimeter_region");
  caloRegion->AddRootLogicalVolume(fLogicCalor);//fLogicCalor);
  // FASTSIM

  fPhysiCalor = new G4PVPlacement(0,                     //no rotation
                                 G4ThreeVector(0, 0, fCalorThickness/2),        //at (0,0,0)
                                 fLogicCalor,            //its fLogical volume
                                 "Calorimeter",          //its name
                                 fLogicWorld,            //its mother  volume
                                 false,                  //no boolean operation
                                 80);                     //copy number

  //
  // Layers
  //

  fSolidLayer = new G4Box("Layer",fCalorSizeYZ/2,fCalorSizeYZ/2,fLayerThickness/2);

  fLogicLayer = new G4LogicalVolume(fSolidLayer,
                                   fWorldMaterial,
                                   "Layer");
  if (fNbOfLayers > 1)
    fPhysiLayer = new G4PVReplica("Layer",
                                  fLogicLayer,
                                  fLogicCalor,
                                  kZAxis,
                                  fNbOfLayers,
                                  fLayerThickness);
  else
    fPhysiLayer = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicLayer,
                                   "Layer",
                                   fLogicCalor,
                                   false,
                                   0);

  //
  // Cells along Y = row
  //

  fSolidRow = new G4Box("Row",fCalorSizeYZ/2,fCalorSizeYZ/fNbOfCells/2,
                       fLayerThickness/2);

  fLogicRow = new G4LogicalVolume(fSolidRow,
                                   fWorldMaterial,
                                   "Row");
  if (fNbOfCells > 1)
    fPhysiRow = new G4PVReplica("Row",
                                fLogicRow,
                                fLogicLayer,
                                 kYAxis,
                                 fNbOfCells,
                                 fCalorSizeYZ/fNbOfCells);
  else
    fPhysiRow = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicRow,
                                   "Row",
                                   fLogicLayer,
                                   false,
                                   0);

  //
  // Cells along Z = single cell
  //

  fSolidCell = new G4Box("Cell", fCalorSizeYZ/fNbOfCells/2,fCalorSizeYZ/fNbOfCells/2,
                       fLayerThickness/2);

  fLogicCell = new G4LogicalVolume(fSolidCell,
                                   fWorldMaterial,
                                   "Cell");
  if (fNbOfCells > 1)
    fPhysiCell = new G4PVReplica("Cell",
                                fLogicCell,
                                fLogicRow,
                                 kXAxis,
                                 fNbOfCells,
                                 fCalorSizeYZ/fNbOfCells);
  else
    fPhysiCell = new G4PVPlacement(0,
                                   G4ThreeVector(),
                                   fLogicCell,
                                   "Cell",
                                   fLogicRow,
                                   false,
                                   0);

  //
  // Absorbers
  //
  G4double xfront = -0.5*fLayerThickness;
  for (G4int k=0; k<fNbOfAbsor; ++k) {
    fSolidAbsor[k] = new G4Box("Absorber",                //its name
                               fCalorSizeYZ/fNbOfCells/2,fCalorSizeYZ/fNbOfCells/2,fAbsorThickness[k]/2);

    fLogicAbsor[k] = new G4LogicalVolume(fSolidAbsor[k],    //its solid
                                        fAbsorMaterial[k], //its material
                                        fAbsorMaterial[k]->GetName());

    G4double xcenter = xfront+0.5*fAbsorThickness[k];
    xfront += fAbsorThickness[k];
    fPhysiAbsor[k] = new G4PVPlacement(0,
                                       G4ThreeVector(0.,0.,xcenter),
                         fLogicAbsor[k],
                         fAbsorMaterial[k]->GetName(),
                         fLogicCell,
                         false,
                         k+50);                                //copy number
  }

  PrintCalorParameters();

  //always return the fPhysical World
  //
  return fPhysiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n-------------------------------------------------------------"
         << "\n ---> The calorimeter is " << fNbOfLayers << " layers of:";
  for (G4int i=0; i<fNbOfAbsor; ++i)
     {
      G4cout << "\n \t" << std::setw(12) << fAbsorMaterial[i]->GetName() <<": "
              << std::setw(6) << G4BestUnit(fAbsorThickness[i],"Length");
     }
  G4cout << "\n-------------------------------------------------------------\n";

  G4cout << "\n" << fWorldMaterial << G4endl;
  for (G4int j=0; j<fNbOfAbsor; j++)
     G4cout << "\n" << fAbsorMaterial[j] << G4endl;

  G4cout << "\n-------------------------------------------------------------\n";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetWorldMaterial(const G4String& material)
{
  // search the material by its name
  G4Material* pttoMaterial =
    G4NistManager::Instance()->FindOrBuildMaterial(material);
  if (pttoMaterial) fWorldMaterial = pttoMaterial;
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfLayers(G4int ival)
{
  // set the number of Layers
  //
  if (ival < 1)
    { G4cout << "\n --->warning from SetfNbOfLayers: "
             << ival << " must be at least 1. Command refused" << G4endl;
      return;
    }
  fNbOfLayers = ival;
}

void DetectorConstruction::SetNbOfCells(G4int ival)
{
  // set the number of cells
  //
  if (ival < 1)
    { G4cout << "\n --->warning from SetfNbOfCells: "
             << ival << " must be at least 1. Command refused" << G4endl;
      return;
    }
  fNbOfCells = ival;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfAbsor(G4int ival)
{
  // set the number of Absorbers
  //
  if (ival < 0 || ival >= kMaxAbsor)
    { G4cout << "\n ---> warning from SetfNbOfAbsor: "
             << ival << " must be at least 1 and and most " << kMaxAbsor-1
             << ". Command refused" << G4endl;
      return;
    }
  fNbOfAbsor = ival;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorMaterial(G4int ival,
              const G4String& material)
{
  // search the material by its name
  //
  if (ival >= fNbOfAbsor || ival < 0)
    { G4cout << "\n --->warning from SetAbsorMaterial: absor number "
             << ival << " out of range. Command refused" << G4endl;
      return;
    }

  G4Material* pttoMaterial =
    G4NistManager::Instance()->FindOrBuildMaterial(material);
  if (pttoMaterial) fAbsorMaterial[ival] = pttoMaterial;
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorThickness(G4int ival,G4double val)
{
  // change Absorber thickness
  //
  if (ival >= fNbOfAbsor || ival < 0)
    { G4cout << "\n --->warning from SetAbsorThickness: absor number "
             << ival << " out of range. Command refused" << G4endl;
      return;
    }
  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetAbsorThickness: thickness "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  fAbsorThickness[ival] = val;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorSensitive(G4int ival,G4bool val)
{
  // change Absorber sensitivity
  //
  std::cout << "SENSITIVE ? " << ival << " = " << val << std::endl;
  fIdOfSD[ival] = val;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCalorSizeYZ(G4double val)
{
  // change the transverse size
  //
  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetfCalorSizeYZ: thickness "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  fCalorSizeYZ = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

void DetectorConstruction::ConstructSDandField()
{
  std::string caloSDname = "ECal";
  test::CalorimeterSD* caloSD = new test::CalorimeterSD(caloSDname,fNbOfCells,
                                                        G4ThreeVector(fCalorSizeYZ/2,fCalorSizeYZ/2,0),
                                                        G4ThreeVector(fCalorSizeYZ/fNbOfCells, fCalorSizeYZ/fNbOfCells, fLayerThickness));
  G4SDManager::GetSDMpointer()->AddNewDetector(caloSD);
  for (uint iAbs = 0; iAbs < fNbOfAbsor; ++iAbs) {
    if (fIdOfSD[iAbs]) {
      SetSensitiveDetector(fLogicAbsor[iAbs], caloSD);
    }
  }
  if ( fFieldMessenger.Get() == 0 ) {
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue = G4ThreeVector();
    G4GlobalMagFieldMessenger* msg =
      new G4GlobalMagFieldMessenger(fieldValue);
    //msg->SetVerboseLevel(1);
    G4AutoDelete::Register(msg);
    fFieldMessenger.Put( msg );
  }

  // FASTSIM
  // for the moment only for homo calorimeters:
  if (fNbOfAbsor == 1) {
    G4RegionStore* regionStore = G4RegionStore::GetInstance();
    G4Region* caloRegion = regionStore->GetRegion("Calorimeter_region");
    G4NistManager* nistManager = G4NistManager::Instance();
    G4cout << "Creating shower parameterization models" << G4endl;
    auto fFastShowerModel = new GFlashShowerModel("fastShowerModel", caloRegion);
    auto fParameterisation = new GFlashHomoShowerParameterisation(fAbsorMaterial[0]);
    fFastShowerModel->SetParameterisation(*fParameterisation);
    // Energy Cuts to kill particles:
    auto fParticleBounds = new GFlashParticleBounds();
    fFastShowerModel->SetParticleBounds(*fParticleBounds);
    // Makes the EnergieSpots
    auto fHitMaker = new GFlashHitMaker();
    fFastShowerModel->SetHitMaker(*fHitMaker);
    G4cout<<"end shower parameterization."<<G4endl;
  }
  // FASTSIM
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
