#include "G4UIcommand.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PrimaryGeneratorMessenger::PrimaryGeneratorMessenger
(PrimaryGeneratorAction* genaction)
  : fPrimaryAction(genaction)
{
  fDir= new G4UIdirectory("/generator/");
  fDir-> SetGuidance("Control commands for primary generator");
  fDirFlat= new G4UIdirectory("/generator/flatDistribution/");
  fDirFlat-> SetGuidance("Control commands for flat energy distribution");

  fSelect= new G4UIcmdWithABool("/generator/singleEnergy", this);
  fSelect-> SetGuidance("single energy instead of flat spectrum");
  fSelect-> SetDefaultValue(false);

  fEnergyMinCmd = new G4UIcmdWithADoubleAndUnit("/generator/flatDistribution/setEnergyMin",this);
  fEnergyMinCmd->SetGuidance("Set minimum energy of a uniform energy distribution");
  fEnergyMinCmd->SetParameterName("Energy",false);
  fEnergyMinCmd->SetRange("Energy>0.");
  fEnergyMinCmd->SetUnitCategory("Energy");
  fEnergyMinCmd->SetDefaultValue(1*CLHEP::GeV);
  fEnergyMaxCmd = new G4UIcmdWithADoubleAndUnit("/generator/flatDistribution/setEnergyMax",this);
  fEnergyMaxCmd->SetGuidance("Set minimum energy of a uniform energy distribution");
  fEnergyMaxCmd->SetParameterName("Energy",false);
  fEnergyMaxCmd->SetRange("Energy>0.");
  fEnergyMaxCmd->SetUnitCategory("Energy");
  fEnergyMinCmd->SetDefaultValue(500*CLHEP::GeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fSelect;
  delete fEnergyMinCmd;
  delete fEnergyMaxCmd;
  delete fDirFlat;
  delete fDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                                 G4String newValues)
{
  if ( command == fSelect ) {
    fPrimaryAction-> SetSingleEnergyMode(fSelect->GetNewBoolValue(newValues));
    G4cout << "single energy mode set to : "
           << fPrimaryAction-> GetSingleEnergyMode() << G4endl;
  } else if (command == fEnergyMinCmd) {
    fPrimaryAction->SetFlatDistributionMinimum(fEnergyMinCmd->GetNewDoubleValue(newValues));
    G4cout << "Setting minimum energy of a flat distribution to " << fPrimaryAction->GetFlatDistributionMinimum() << G4endl;
  } else if (command == fEnergyMaxCmd) {
    fPrimaryAction->SetFlatDistributionMaximum(fEnergyMaxCmd->GetNewDoubleValue(newValues));
    G4cout << "Setting maximum energy of a flat distribution to " << fPrimaryAction->GetFlatDistributionMaximum() << G4endl;
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4String PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4bool mode;
  if ( command == fSelect ) {
    mode = fPrimaryAction-> GetSingleEnergyMode();
  }

  return mode;
}
