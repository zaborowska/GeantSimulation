#ifndef DEBUGKILLPROCESS_H
#define DEBUGKILLPROCESS_H

// Geant
#include "G4VProcess.hh"

class DebugKillProcessMessenger;

class DebugKillProcess : public G4VProcess {
public:
  /// Constructor.
  DebugKillProcess();
  /// Destructor.
  virtual ~DebugKillProcess(){};
  /// Add the custom process that deposits all energy in the vertex.
  virtual G4VParticleChange* AtRestDoIt(const G4Track&, const G4Step&) final { return nullptr; };
  virtual G4double AtRestGetPhysicalInteractionLength(const G4Track&, G4ForceCondition*) final { return -1; };
  virtual G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&) final;
  virtual G4double PostStepGetPhysicalInteractionLength(const G4Track& aTrack, G4double, G4ForceCondition* aCondition) final {
    *aCondition = Forced;
    double energy = aTrack.GetTotalEnergy();
    if (energy <= fEnergyThreshold ) {
      G4cout << " Will try to kill particle and deposit ALL energy " << aTrack.GetTotalEnergy() << G4endl;
      return 0;
    }
    *aCondition = InActivated;
    return DBL_MAX;
  };
  virtual G4VParticleChange* AlongStepDoIt(const G4Track&, const G4Step&) final { return nullptr; };
  virtual G4double AlongStepGetPhysicalInteractionLength(const G4Track&, G4double, G4double, G4double&,
                                                         G4GPILSelection*) final {
    return -1;
  };
  inline void SetEnergyThreshold(G4double aEnergy) {fEnergyThreshold = aEnergy;};
  inline G4double GetEnergyThreshold() const {return fEnergyThreshold;};
private:
  G4double fEnergyThreshold;
  DebugKillProcessMessenger* fMessenger;
};

#endif
