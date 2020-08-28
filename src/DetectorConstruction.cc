#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4VisAttributes.hh"

#include "G4GeometryManager.hh"
#include "G4SDManager.hh"

#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4PhysicalConstants.hh"

#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
 :G4VUserDetectorConstruction(),
  fWorldMaterial(nullptr),fSolidWorld(nullptr),fLogicWorld(nullptr),
  fPhysiWorld(nullptr),fSolidCalor(nullptr),fLogicCalor(nullptr),
  fPhysiCalor(nullptr),fSolidLayer(nullptr),fLogicLayer(nullptr),
  fPhysiLayer(nullptr),fDetectorMessenger(nullptr),
  fParametersFileName("")
{
  fDetectorMessenger= new DetectorMessenger(this);

  // default parameter values of the calorimeter
  fNbOfAbsor = 1;
  fAbsorThickness[0] = 100*mm;
  fNbOfLayers        = 5;
  fNbOfRhoCells         = 5;
  fNbOfPhiCells         = 1;
  fCalorRadius       = 50*mm * fNbOfRhoCells;

  // materials
  SetWorldMaterial("G4_Galactic");
  SetAbsorMaterial(0,"G4_PbWO4");
  SetAbsorSensitive(0,true);
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
  fWorldSizeZ = 2.2*fCalorThickness;
  fWorldSizeTransverse = 2.2*fCalorRadius;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  if(fPhysiWorld) { return fPhysiWorld; }
  ComputeCalorParameters();

  fSolidWorld = new G4Box("World",                                //its name
                   fWorldSizeTransverse/2,fWorldSizeTransverse/2,fWorldSizeZ/2);  //its size

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

  G4double full2Pi = 2.* M_PI * rad;
  fSolidCalor = new G4Tubs("Calorimeter",0,fCalorRadius, fCalorThickness/2, 0, full2Pi);

  fLogicCalor = new G4LogicalVolume(fSolidCalor,
                                         fWorldMaterial,
                                         "Calorimeter");

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

  fSolidLayer = new G4Tubs("Layer",0,fCalorRadius,fLayerThickness/2, 0,  full2Pi);

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
  // Absorbers
  //
  G4double xfront = -0.5*fLayerThickness;
  for (G4int k=0; k<fNbOfAbsor; ++k) {
    fSolidAbsor[k] = new G4Tubs("Absorber",                //its name
                                0,fCalorRadius,fAbsorThickness[k]/2,0,full2Pi);

    fLogicAbsor[k] = new G4LogicalVolume(fSolidAbsor[k],    //its solid
                                        fAbsorMaterial[k], //its material
                                        fAbsorMaterial[k]->GetName());
    fLogicAbsor[k]->SetVisAttributes(G4Colour (0.5, 0.5, 0.5, 0.2));

    G4double xcenter = xfront+0.5*fAbsorThickness[k];
    xfront += fAbsorThickness[k];
    fPhysiAbsor[k] = new G4PVPlacement(0,
                                       G4ThreeVector(0.,0.,xcenter),
                         fLogicAbsor[k],
                         fAbsorMaterial[k]->GetName(),
                         fLogicLayer,
                         false,
                         k+50);                                //copy number
  }

  // visualisation settings
  fLogicWorld->SetVisAttributes(G4VisAttributes::Invisible);
  fLogicCalor->SetVisAttributes(G4VisAttributes::Invisible);
  fLogicLayer->SetVisAttributes(G4VisAttributes::Invisible);

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

void DetectorConstruction::SetNbOfRhoCells(G4int ival)
{
  // set the number of cells
  //
  if (ival < 1)
    { G4cout << "\n --->warning from SetfNbOfRhoCells: "
             << ival << " must be at least 1. Command refused" << G4endl;
      return;
    }
  fNbOfRhoCells = ival;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfPhiCells(G4int ival)
{
  // set the number of cells
  //
  if (ival < 1)
    { G4cout << "\n --->warning from SetfNbOfPhiCells: "
             << ival << " must be at least 1. Command refused" << G4endl;
      return;
    }
  fNbOfPhiCells = ival;
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

void DetectorConstruction::SetCalorRadius(G4double val)
{
  // change the transverse size
  //
  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetfCalorRadius: thickness "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  fCalorRadius = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

void DetectorConstruction::ConstructSDandField()
{
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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
