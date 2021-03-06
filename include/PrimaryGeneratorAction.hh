#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include <fstream>

class G4ParticleGun;
class G4Event;
class G4Box;
class PrimaryGeneratorMessenger;

/// The primary generator action class with particle gun with enegy read from file
///

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    // method from the base class
    virtual void GeneratePrimaries(G4Event*);

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  void SetSingleEnergyMode(G4bool aSingle) {fSingleEnergy = aSingle;}
  G4bool GetSingleEnergyMode() const {return fSingleEnergy;}
  void SetFlatDistributionMinimum(G4double aEnergy) {fFlatDistributionMin = aEnergy;}
  G4double GetFlatDistributionMinimum() const {return fFlatDistributionMin;}
  void SetFlatDistributionMaximum(G4double aEnergy) {fFlatDistributionMax = aEnergy;}
  G4double GetFlatDistributionMaximum() const {return fFlatDistributionMax;}
    std::ifstream in;

  private:
    G4ParticleGun*  fParticleGun; // pointer a to G4 gun class
  PrimaryGeneratorMessenger* fMessenger;
  G4bool fSingleEnergy;
  G4double fFlatDistributionMin;
  G4double fFlatDistributionMax;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
