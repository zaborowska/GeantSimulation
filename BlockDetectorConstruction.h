#ifndef BlockDetectorConstruction_h
#define BlockDetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "globals.hh"
#include <string>

class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;
class G4GenericMessenger;

namespace test {
class BlockDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    BlockDetectorConstruction();
    BlockDetectorConstruction(double,double,std::string);
    BlockDetectorConstruction(double , double , std::string ,double , double , double  , double );
    virtual ~BlockDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    void ConstructMaterials();
    inline int GetNumberOfCellsRad() {std::cout <<"RAD SIZE " << fNumCellsRad << std::endl; return fNumCellsRad;}
    inline int GetNumberOfCellsZ() {std::cout << "Z SIZE " << fNumCellsZ << std::endl; return fNumCellsZ;}

private:
    G4GenericMessenger* fMessenger;
    G4LogicalVolume* fCaloLogical;
    int fBoxSizeInRad;
    int fBoxSizeInZ;
    double fCellSizeInRad;
    double fCellSizeInZ;
    double fRadLen;
    double fMolRad;
    double fNumCellsRad;
    double fNumCellsZ;
    std::string fMaterial;
};
}

#endif /* BlockDetectorConstruction_h */
