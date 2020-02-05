#include "DebugKillProcessMessenger.h"

#include "DebugKillProcess.h"

#include "G4UIcmdWithADoubleAndUnit.hh"

DebugKillProcessMessenger::DebugKillProcessMessenger(DebugKillProcess* process)
: G4UImessenger(), fDebugKillProcess(process), fEnergyThresholdCmd(nullptr)
{
  fEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/debug/kill/energyThreshold",this);
  fEnergyThresholdCmd->SetGuidance("Set max allowed step length");
  fEnergyThresholdCmd->SetParameterName("energy",false);
  fEnergyThresholdCmd->SetRange("energy>0.");
  fEnergyThresholdCmd->SetUnitCategory("Energy");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DebugKillProcessMessenger::~DebugKillProcessMessenger()
{
  delete fEnergyThresholdCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DebugKillProcessMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fEnergyThresholdCmd)
    { fDebugKillProcess->SetEnergyThreshold(fEnergyThresholdCmd->GetNewDoubleValue(newValue));}
}
