#include "BlockDetectorConstruction.h"
#include "CalorimeterSD.h"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

// take size of box: 45 X0 and 10 RM

namespace test {
   BlockDetectorConstruction::BlockDetectorConstruction()
      : G4VUserDetectorConstruction(),
        fMessenger(0),
        fCaloLogical(0),
        fMaterial("G4_Pb"),
        fRadLen(5.612),
        fMolRad(16.02),
        fCellSizeInZ(0.1),
        fCellSizeInRad(1),
        fBoxSizeInZ(120),
        fBoxSizeInRad(30)
   {
      fNumCellsRad = fBoxSizeInRad / fCellSizeInRad;
      fNumCellsZ = fBoxSizeInZ / fCellSizeInZ;
   }

   BlockDetectorConstruction::BlockDetectorConstruction(double radLen, double molRad, std::string material)
      : G4VUserDetectorConstruction(),
        fMessenger(0),
        fCaloLogical(0),
        fMaterial(material),
        fRadLen(radLen),
        fMolRad(molRad),
        fCellSizeInZ(0.1),
        fCellSizeInRad(1),
        fBoxSizeInZ(120),
        fBoxSizeInRad(30)
   {
      fNumCellsRad = fBoxSizeInRad / fCellSizeInRad;
      fNumCellsZ = fBoxSizeInZ / fCellSizeInZ;
   }
   BlockDetectorConstruction::BlockDetectorConstruction(double radLen, double molRad, std::string material,
                                                        double cellSizeInZ, double cellSizeInRad, double  boxSizeInZ, double boxSizeInRad)
      : G4VUserDetectorConstruction(),
        fMessenger(0),
        fCaloLogical(0),
        fMaterial(material),
        fRadLen(radLen),
        fMolRad(molRad),
        fCellSizeInZ(cellSizeInZ),
        fCellSizeInRad(cellSizeInRad),
        fBoxSizeInZ(boxSizeInZ),
        fBoxSizeInRad(boxSizeInRad)
   {
      fNumCellsRad = fBoxSizeInRad / fCellSizeInRad;
      fNumCellsZ = fBoxSizeInZ / fCellSizeInZ;
   }

   BlockDetectorConstruction::~BlockDetectorConstruction() {
      delete fMessenger;
   }

   G4VPhysicalVolume* BlockDetectorConstruction::Construct() {
      // Construct materials
      ConstructMaterials();
      G4Material* air = G4Material::GetMaterial("G4_AIR");
      G4NistManager* nistManager = G4NistManager::Instance();
      nistManager->FindOrBuildMaterial(fMaterial);
      G4Material* blockMaterial = G4Material::GetMaterial(fMaterial);

      // Option to switch on/off checking of volumes overlaps
      G4bool checkOverlaps = true;

      // set geometry parameters
      double fBoxSizeRad = fMolRad * fBoxSizeInRad;
      double fBoxSizeZ = fRadLen * fBoxSizeInZ;
      double fCellSizeRad = fMolRad * fCellSizeInRad;
      double fCellSizeZ = fRadLen * fCellSizeInZ;
      G4ThreeVector fInitialPos(0, 0, 0); // to make its one corner in 0,0,0
      int cell_noRad = fNumCellsRad ;//fBoxSizeRad/fCellSizeRad;
      double half_boxRad = fBoxSizeRad/2.;
      double half_cellRad = fCellSizeRad/2.;
      int cell_noZ = fNumCellsZ ;//fBoxSizeZ/fCellSizeZ;
      double half_boxZ = fBoxSizeZ/2.;
      double half_cellZ = fCellSizeZ/2.;


      // geometries --------------------------------------------------------------
      // experimental hall (world volume)
      G4VSolid* worldSolid
         = new G4Box("worldBox",2*half_boxRad,2*half_boxRad,2*half_boxZ);
      G4LogicalVolume* worldLogical
         = new G4LogicalVolume(worldSolid,air,"worldLogical");
      G4VPhysicalVolume* worldPhysical
         = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,
                             false,0,checkOverlaps);


      // hadron calorimeter
      G4VSolid* hadCalorimeterSolid
         = new G4Box("calorimeterBox",half_boxRad,half_boxRad,half_boxZ);
      G4LogicalVolume* hadCalorimeterLogical
         = new G4LogicalVolume(hadCalorimeterSolid,blockMaterial,"calorimeterLogical");
      new G4PVPlacement(0,fInitialPos,hadCalorimeterLogical,
                        "calorimeterPhysical",worldLogical,
                        false,0,checkOverlaps);

      // hadron calorimeter column
      G4VSolid* calColumnSolid
         = new G4Box("calColumnBox",half_cellRad,half_boxRad,half_boxZ);
      G4LogicalVolume* calColumnLogical
         = new G4LogicalVolume(calColumnSolid,blockMaterial,"calColumnLogical");
      new G4PVReplica("calColumnPhysical",calColumnLogical,
                      hadCalorimeterLogical,kXAxis,cell_noRad,fCellSizeRad);

      // hadron calorimeter cell
      G4VSolid* calCellSolid
         = new G4Box("calCellBox",half_cellRad,half_cellRad,half_boxZ);
      G4LogicalVolume* calCellLogical
         = new G4LogicalVolume(calCellSolid,blockMaterial,"calCellLogical");
      new G4PVReplica("calCellPhysical",calCellLogical,
                      calColumnLogical,kYAxis,cell_noRad,fCellSizeRad);

      // hadron calorimeter layers
      G4VSolid* calLayerSolid
         = new G4Box("calLayerBox",half_cellRad,half_cellRad,half_cellZ );
      fCaloLogical
         = new G4LogicalVolume(calLayerSolid,blockMaterial,"calLayerLogical");
      new G4PVReplica("calLayerPhysical",fCaloLogical,
                      calCellLogical,kZAxis,cell_noZ,fCellSizeZ);

      return worldPhysical;
   }

   void BlockDetectorConstruction::ConstructSDandField() {
      // sensitive detectors -----------------------------------------------------
      G4SDManager* SDman = G4SDManager::GetSDMpointer();
      G4String SDname;

      G4VSensitiveDetector* calorimeterSD
         = new CalorimeterSD(SDname="/Calorimeter", fBoxSizeInRad/fCellSizeInRad, fBoxSizeInRad/fCellSizeInRad, fBoxSizeInZ/fCellSizeInZ);
      std::cout<<"Sensitive detector created with "<<fBoxSizeInRad/fCellSizeInRad<<"x"<<fBoxSizeInRad/fCellSizeInRad<<"x"<<fBoxSizeInZ/fCellSizeInZ<<"x"<<" cells"<<std::endl;
      SDman->AddNewDetector(calorimeterSD);
      fCaloLogical->SetSensitiveDetector(calorimeterSD);

   }

   void BlockDetectorConstruction::ConstructMaterials() {
      G4NistManager* nistManager = G4NistManager::Instance();

      // Air
      nistManager->FindOrBuildMaterial("G4_AIR");

      G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
      G4cout << *(G4Material::GetMaterialTable()) << G4endl;
   }

}
