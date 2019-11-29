#include "DetectorMessenger.hh"

#include <sstream>

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction * Det)
:G4UImessenger(),fDetector(Det),
 fTestemDir(nullptr),
 fDetDir(nullptr),
 fSizeCaloRadiusCmd(nullptr),
 fNbLayersCmd(nullptr),
 fNbRhoCellsCmd(nullptr),
 fNbPhiCellsCmd(nullptr),
 fSensitiveCmd(nullptr),
 fNotSensitiveCmd(nullptr),
 fNbAbsorCmd(nullptr),
 fAbsorCmd(nullptr),
 fParametersNameCmd(nullptr)
{
  fTestemDir = new G4UIdirectory("/testem/");
  fTestemDir->SetGuidance("UI commands specific to this example");

  fDetDir = new G4UIdirectory("/testem/det/");
  fDetDir->SetGuidance("detector construction commands");

  fSizeCaloRadiusCmd = new G4UIcmdWithADoubleAndUnit("/testem/det/setCaloRadius",this);
  fSizeCaloRadiusCmd->SetGuidance("Set tranverse size of the calorimeter (radius)");
  fSizeCaloRadiusCmd->SetParameterName("Size",false);
  fSizeCaloRadiusCmd->SetRange("Size>0.");
  fSizeCaloRadiusCmd->SetUnitCategory("Length");
  fSizeCaloRadiusCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fSizeCaloRadiusCmd->SetToBeBroadcasted(false);

  fNbLayersCmd = new G4UIcmdWithAnInteger("/testem/det/setNbOfLayers",this);
  fNbLayersCmd->SetGuidance("Set number of layers.");
  fNbLayersCmd->SetParameterName("NbLayers",false);
  fNbLayersCmd->SetRange("NbLayers>0");
  fNbLayersCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fNbLayersCmd->SetToBeBroadcasted(false);

  fNbRhoCellsCmd = new G4UIcmdWithAnInteger("/testem/det/setNbOfRhoCells",this);
  fNbRhoCellsCmd->SetGuidance("Set number of cells.");
  fNbRhoCellsCmd->SetParameterName("NbRhoCells",false);
  fNbRhoCellsCmd->SetRange("NbRhoCells>0");
  fNbRhoCellsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fNbRhoCellsCmd->SetToBeBroadcasted(false);

  fNbPhiCellsCmd = new G4UIcmdWithAnInteger("/testem/det/setNbOfPhiCells",this);
  fNbPhiCellsCmd->SetGuidance("Set number of cells.");
  fNbPhiCellsCmd->SetParameterName("NbPhiCells",false);
  fNbPhiCellsCmd->SetRange("NbPhiCells>0");
  fNbPhiCellsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fNbPhiCellsCmd->SetToBeBroadcasted(false);

  fNbAbsorCmd = new G4UIcmdWithAnInteger("/testem/det/setNbOfAbsor",this);
  fNbAbsorCmd->SetGuidance("Set number of Absorbers.");
  fNbAbsorCmd->SetParameterName("NbAbsor",false);
  fNbAbsorCmd->SetRange("NbAbsor>0");
  fNbAbsorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fNbAbsorCmd->SetToBeBroadcasted(false);

  fAbsorCmd = new G4UIcommand("/testem/det/setAbsor",this);
  fAbsorCmd->SetGuidance("Set the absor nb, the material, the thickness.");
  fAbsorCmd->SetGuidance("  absor number : from 1 to NbOfAbsor");
  fAbsorCmd->SetGuidance("  material name");
  fAbsorCmd->SetGuidance("  thickness (with unit) : t>0");
  fAbsorCmd->SetGuidance("  if sensitive : true/false.");
  //
  G4UIparameter* AbsNbPrm = new G4UIparameter("AbsorNb",'i',false);
  AbsNbPrm->SetGuidance("absor number : from 1 to NbOfAbsor");
  AbsNbPrm->SetParameterRange("AbsorNb>-1");
  fAbsorCmd->SetParameter(AbsNbPrm);
  //
  G4UIparameter* MatPrm = new G4UIparameter("material",'s',false);
  MatPrm->SetGuidance("material name");
  fAbsorCmd->SetParameter(MatPrm);
  //
  G4UIparameter* ThickPrm = new G4UIparameter("thickness",'d',false);
  ThickPrm->SetGuidance("thickness of absorber");
  ThickPrm->SetParameterRange("thickness>0.");
  fAbsorCmd->SetParameter(ThickPrm);
  //
  G4UIparameter* unitPrm = new G4UIparameter("unit",'s',false);
  unitPrm->SetGuidance("unit of thickness");
  G4String unitList = G4UIcommand::UnitsList(G4UIcommand::CategoryOf("mm"));
  unitPrm->SetParameterCandidates(unitList);
  fAbsorCmd->SetParameter(unitPrm);
  //
  G4UIparameter* SensitivePrm = new G4UIparameter("sensitive",'b',false);
  SensitivePrm->SetGuidance("if absorber is sensitive (registers energy deposits)");
  fAbsorCmd->SetParameter(SensitivePrm);
  //
  fAbsorCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fAbsorCmd->SetToBeBroadcasted(false);
  //
  fParametersNameCmd = new G4UIcmdWithAString("/testem/det/setParametrisationFile",this);
  fParametersNameCmd->SetGuidance("Path to the parametrisation input for EM showers.");
  fParametersNameCmd->SetParameterName("FileName",false);
  fParametersNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  fParametersNameCmd->SetToBeBroadcasted(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fSizeCaloRadiusCmd;
  delete fNbLayersCmd;
  delete fNbRhoCellsCmd;
  delete fNbPhiCellsCmd;
  delete fSensitiveCmd;
  delete fNotSensitiveCmd;
  delete fNbAbsorCmd;
  delete fAbsorCmd;
  delete fParametersNameCmd;
  delete fDetDir;
  delete fTestemDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == fSizeCaloRadiusCmd )
   { fDetector->SetCalorRadius(fSizeCaloRadiusCmd->GetNewDoubleValue(newValue));}

  else if( command == fNbLayersCmd )
    { fDetector->SetNbOfLayers(fNbLayersCmd->GetNewIntValue(newValue));}

  else if( command == fNbRhoCellsCmd )
   { fDetector->SetNbOfRhoCells(fNbRhoCellsCmd->GetNewIntValue(newValue));}

  else if( command == fNbPhiCellsCmd )
   { fDetector->SetNbOfPhiCells(fNbPhiCellsCmd->GetNewIntValue(newValue));}

  else if( command == fNbAbsorCmd )
   { fDetector->SetNbOfAbsor(fNbAbsorCmd->GetNewIntValue(newValue));}

  else if (command == fAbsorCmd)
   {
     G4int num; G4double tick;
     G4String unt, mat;
     G4bool sensitive;
     std::istringstream is(newValue);
     is >> num >> mat >> tick >> unt >> std::boolalpha >> sensitive;
     std::cout << "params: " << newValue << std::endl;
     std::cout << "params: " << num << "\t" << mat << "\t" << tick << "\t" << unt << "\t" << sensitive << std::endl;
     G4String material=mat;
     tick *= G4UIcommand::ValueOf(unt);
     fDetector->SetAbsorMaterial (num,material);
     fDetector->SetAbsorThickness(num,tick);
     fDetector->SetAbsorSensitive(num,sensitive);
   }
  else if( command == fParametersNameCmd )
   { fDetector->SetParametersFileName(newValue);}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
