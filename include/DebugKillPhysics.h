#ifndef DEBUGKILLPHYSICS_H
#define DEBUGKILLPHYSICS_H

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class DebugKillPhysics : public G4VPhysicsConstructor
{
public:

  DebugKillPhysics(const G4String& name = "debugKill");
  virtual ~DebugKillPhysics();
public:
  // -- Set energy threshold below which to kill particles
  void SetEnergyThreshold(const G4double aEnergyThreshold);
  // -- Information about particles under fast simulation:
  void BeVerbose() { fVerbose = true; }

  // This method is dummy for physics
  virtual void ConstructParticle();

  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type
  virtual void ConstructProcess();

private:
// -- Energy threshold
  G4double fEnergyThreshold;

  // -- Report:
  G4bool fVerbose;

};

#endif

