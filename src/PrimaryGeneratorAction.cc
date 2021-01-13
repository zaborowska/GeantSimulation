#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <filesystem>

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr),
  fSingleEnergy(false),
  fFlatDistributionMin(1 * GeV),
  fFlatDistributionMax(500 * GeV)
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
  fMessenger= new PrimaryGeneratorMessenger(this);
  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="e-");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(100.*GeV);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));

}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
#if defined(RICHVIS) || defined(REPRODUCESEEDS)
  std::string fileName = "random/event_"+std::to_string(anEvent->GetEventID())+".rndm.stat";
  if (std::filesystem::exists(fileName)) {
    CLHEP::HepRandom::getTheEngine()->restoreStatus (fileName.c_str());
  } else {
    std::filesystem::create_directory("random");
    CLHEP::HepRandom::getTheEngine()->saveStatus (fileName.c_str());
  }
#endif
  if (!fSingleEnergy) {
    G4double u = G4UniformRand();
    fParticleGun->SetParticleEnergy(fFlatDistributionMin + (u * (fFlatDistributionMax - fFlatDistributionMin))); // get random number from min to max
  }
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

