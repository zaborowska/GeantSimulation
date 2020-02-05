#include "DebugKillPhysics.h"
#include "DebugKillProcess.h"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4UnitsTable.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"


// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(DebugKillPhysics);

DebugKillPhysics::DebugKillPhysics(const G4String& name)
  :  G4VPhysicsConstructor(name),
     fEnergyThreshold(1.),
     fVerbose(true)
{;}

DebugKillPhysics::~DebugKillPhysics()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DebugKillPhysics::SetEnergyThreshold(const G4double aEnergyThreshold)
{
  fEnergyThreshold = aEnergyThreshold;
}

void DebugKillPhysics::ConstructParticle()
{;}

void DebugKillPhysics::ConstructProcess()
{
  DebugKillProcess* killProcess = new DebugKillProcess();
  auto myParticleIterator = GetParticleIterator();
  myParticleIterator->reset();
  while ( (*myParticleIterator)() )
    {
      G4ParticleDefinition*     particle = myParticleIterator->value();
      G4ProcessManager*         pmanager = particle->GetProcessManager();
      if ( particle == G4Electron::ElectronDefinition() ||
           particle == G4Positron::PositronDefinition() ||
           particle == G4Gamma::GammaDefinition() )
        {
          pmanager-> AddDiscreteProcess(killProcess);
        }
    }

  // -- tells what is done:
  if ( fVerbose )
    {
      // -- print:
      myParticleIterator->reset();
      while ( (*myParticleIterator)() )
        {
          G4ParticleDefinition*     particle = myParticleIterator->value();
          G4String              particleName = particle->GetParticleName();
          G4ProcessManager*         pmanager = particle->GetProcessManager();
          G4ProcessVector*  vprocess = pmanager->GetProcessList();
          for (size_t ip = 0 ; ip < vprocess->size() ; ip++)
            {
              G4VProcess* process = (*vprocess)[ip];
              DebugKillProcess* proc = dynamic_cast< DebugKillProcess* >(process);
              if ( proc != nullptr )
                {
                  G4cout << std::setw(14) << particleName << " : kill particle if energy < "
                         << G4BestUnit(fEnergyThreshold, "Energy") << G4endl;
                }
            }
        }
    }
}
