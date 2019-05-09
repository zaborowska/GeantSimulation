#include "G4UIcommand.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
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

  fSelect= new G4UIcmdWithABool("/generator/singleEnergy", this);
  fSelect-> SetGuidance("single energy instead of flat spectrum");
  fSelect-> SetDefaultValue(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete fSelect;
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
