#include "DebugKillProcessMessenger.h"

#include "DebugKillProcess.h"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

DebugKillProcessMessenger::DebugKillProcessMessenger(DebugKillProcess* process)
: G4UImessenger(), fDebugKillProcess(process), fEnergyThresholdCmd(nullptr), fActivateCmd(nullptr)
{
  fEnergyThresholdCmd = new G4UIcmdWithADoubleAndUnit("/debug/kill/energyThreshold",this);
  fEnergyThresholdCmd->SetGuidance("Set max allowed step length");
  fEnergyThresholdCmd->SetParameterName("energy",false);
  fEnergyThresholdCmd->SetRange("energy>0.");
  fEnergyThresholdCmd->SetUnitCategory("Energy");

  fActivateCmd = new G4UIcmdWithABool("/debug/kill/activate",this);
  fActivateCmd->SetGuidance("Defines if killing process is activated");
  fActivateCmd->SetParameterName("activate",false,false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DebugKillProcessMessenger::~DebugKillProcessMessenger()
{
  delete fEnergyThresholdCmd;
  delete fActivateCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DebugKillProcessMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fEnergyThresholdCmd) {
    fDebugKillProcess->SetEnergyThreshold(fEnergyThresholdCmd->GetNewDoubleValue(newValue));
  }
  else if( command == fActivateCmd ) {
    fDebugKillProcess->SetActivated(fActivateCmd->GetNewBoolValue(newValue));
  }
}
