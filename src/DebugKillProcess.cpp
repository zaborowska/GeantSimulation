#include "DebugKillProcess.h"
#include "DebugKillProcessMessenger.h"

DebugKillProcess::DebugKillProcess() : G4VProcess("debugKill", fUserDefined) {
  fEnergyThreshold = 1; // default threshold to kill: 1 MeV
  fActivated = false; // by default switched off
  fMessenger = new DebugKillProcessMessenger(this);
}

G4VParticleChange* DebugKillProcess::PostStepDoIt(const G4Track& aTrack, const G4Step&) {
  G4VParticleChange* finalState = new G4VParticleChange();
  finalState->Initialize(aTrack);
  finalState->ClearDebugFlag();
  finalState->ProposeLocalEnergyDeposit(aTrack.GetTotalEnergy());
  finalState->SetNumberOfSecondaries(0);
  finalState->ProposeTrackStatus(fStopAndKill);
  G4cout << " Killing particle and depositing ALL energy " << aTrack.GetTotalEnergy() << G4endl;
  return finalState;
}
