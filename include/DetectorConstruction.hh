#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Cache.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class DetectorMessenger;

class G4GlobalMagFieldMessenger;

const G4int kMaxAbsor = 10;                        // 0 + 9

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();

  void SetNbOfAbsor     (G4int);
  void SetAbsorMaterial (G4int,const G4String&);
  void SetAbsorThickness(G4int,G4double);
  void SetAbsorSensitive(G4int,G4bool);
  void SetWorldMaterial (const G4String&);
  void SetCalorSizeYZ   (G4double);
  void SetNbOfLayers    (G4int);
  void SetNbOfCells    (G4int);

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  void PrintCalorParameters();

  G4double GetWorldSizeX() const     {return fWorldSizeX;};
  G4double GetWorldSizeYZ() const    {return fWorldSizeYZ;};

  G4double GetCalorThickness() const {return fCalorThickness;};
  G4double GetCalorSizeYZ() const    {return fCalorSizeYZ;};

  G4int GetNbOfLayers() const        {return fNbOfLayers;};

  G4int       GetNbOfAbsor() const   {return fNbOfAbsor;};
  const G4Material* GetAbsorMaterial(G4int i) const
  {return fAbsorMaterial[i];};
  G4double GetAbsorThickness(G4int i) const
  {return fAbsorThickness[i];};

  const G4VPhysicalVolume* GetphysiWorld()        {return fPhysiWorld;};
  const G4Material*        GetWorldMaterial()     {return fWorldMaterial;};
  const G4VPhysicalVolume* GetAbsorber(G4int i)   {return fPhysiAbsor[i];};

private:

  G4int              fNbOfAbsor;
  G4Material*        fAbsorMaterial [kMaxAbsor];
  G4double           fAbsorThickness[kMaxAbsor];

  G4int              fNbOfLayers;
  G4int              fNbOfCells;
  G4bool             fIdOfSD[kMaxAbsor];
  G4double           fLayerThickness;

  G4double           fCalorSizeYZ;
  G4double           fCalorThickness;

  G4Material*        fWorldMaterial;
  G4double           fWorldSizeYZ;
  G4double           fWorldSizeX;

  G4Box*             fSolidWorld;
  G4LogicalVolume*   fLogicWorld;
  G4VPhysicalVolume* fPhysiWorld;

  G4Box*             fSolidCalor;
  G4LogicalVolume*   fLogicCalor;
  G4VPhysicalVolume* fPhysiCalor;

  G4Box*             fSolidRow;
  G4LogicalVolume*   fLogicRow;
  G4VPhysicalVolume* fPhysiRow;

  G4Box*             fSolidCell;
  G4LogicalVolume*   fLogicCell;
  G4VPhysicalVolume* fPhysiCell;

  G4Box*             fSolidLayer;
  G4LogicalVolume*   fLogicLayer;
  G4VPhysicalVolume* fPhysiLayer;

  G4Box*             fSolidAbsor[kMaxAbsor];
  G4LogicalVolume*   fLogicAbsor[kMaxAbsor];
  G4VPhysicalVolume* fPhysiAbsor[kMaxAbsor];

  DetectorMessenger* fDetectorMessenger;
  G4Cache<G4GlobalMagFieldMessenger*> fFieldMessenger;

private:

  void ComputeCalorParameters();
};

#endif
