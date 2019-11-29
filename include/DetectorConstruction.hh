#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Cache.hh"

class G4Box;
class G4Tubs;
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
  void SetCalorRadius   (G4double);
  void SetNbOfLayers    (G4int);
  void SetNbOfRhoCells    (G4int);
  void SetNbOfPhiCells    (G4int);

  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  void PrintCalorParameters();

  G4double GetWorldSizeTransverse() const     {return fWorldSizeTransverse;};
  G4double GetWorldSizeZ() const    {return fWorldSizeZ;};

  G4double GetCalorThickness() const {return fCalorThickness;};
  G4double GetCalorRadius() const    {return fCalorRadius;};

  G4int GetNbOfLayers() const        {return fNbOfLayers;};
  G4int GetNbOfRhoCells() const        {return fNbOfRhoCells;};
  G4int GetNbOfPhiCells() const        {return fNbOfPhiCells;};

  G4int       GetNbOfAbsor() const   {return fNbOfAbsor;};
  G4Material* GetAbsorMaterial(G4int i) const // return of non-const pointer for gflash to work
  {return fAbsorMaterial[i];};
  G4double GetAbsorThickness(G4int i) const {return fAbsorThickness[i];};

  const G4VPhysicalVolume* GetphysiWorld()        {return fPhysiWorld;};
  const G4Material*        GetWorldMaterial()     {return fWorldMaterial;};
  const G4VPhysicalVolume* GetAbsorber(G4int i)   {return fPhysiAbsor[i];};
  inline const G4String GetParametersFileName() const {return fParametersFileName;};
  inline void SetParametersFileName(const G4String& aFileName) {fParametersFileName = aFileName;};

private:

  G4int              fNbOfAbsor;
  G4Material*        fAbsorMaterial [kMaxAbsor];
  G4double           fAbsorThickness[kMaxAbsor];

  G4int              fNbOfLayers;
  G4int              fNbOfRhoCells;
  G4int              fNbOfPhiCells;
  G4bool             fIdOfSD[kMaxAbsor];
  G4double           fLayerThickness;

  G4double           fCalorRadius;
  G4double           fCalorThickness;

  G4Material*        fWorldMaterial;
  G4double           fWorldSizeTransverse;
  G4double           fWorldSizeZ;

  G4Box*             fSolidWorld;
  G4LogicalVolume*   fLogicWorld;
  G4VPhysicalVolume* fPhysiWorld;

  G4Tubs*             fSolidCalor;
  G4LogicalVolume*   fLogicCalor;
  G4VPhysicalVolume* fPhysiCalor;

  G4Tubs*             fSolidLayer;
  G4LogicalVolume*   fLogicLayer;
  G4VPhysicalVolume* fPhysiLayer;

  G4Tubs*             fSolidAbsor[kMaxAbsor];
  G4LogicalVolume*   fLogicAbsor[kMaxAbsor];
  G4VPhysicalVolume* fPhysiAbsor[kMaxAbsor];

  DetectorMessenger* fDetectorMessenger;
  G4Cache<G4GlobalMagFieldMessenger*> fFieldMessenger;

  G4String fParametersFileName;

private:

  void ComputeCalorParameters();
};

#endif
