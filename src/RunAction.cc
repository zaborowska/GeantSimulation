#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <valgrind/callgrind.h>
#include <filesystem>

RunAction::RunAction(SaveToFileEventAction* eventAction)
  : G4UserRunAction(),
    fEventAction(eventAction) {
  G4RunManager::GetRunManager()->SetPrintProgress(1000);

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Create directories
  analysisManager->SetNtupleMerging(true);
  analysisManager->SetVerboseLevel(1);
  
  // Creating histograms
  G4double maxEnergy = 500 * GeV;
  G4double maxLong = 500 * mm;
  G4double maxTrans = 100 * mm;
  G4int numLong = 200;
  G4int numTrans = 1000;
  analysisManager->CreateH1("EnergyParticle", "Primary energy;E_{MC} (MeV);Normalised entries", 1000, 0, 1.1 * maxEnergy);
  analysisManager->CreateH1("EnergyDep", "Deposited energy;E_{MC} (MeV);Normalised entries", 1000, 0, 1.1 * maxEnergy);
  analysisManager->CreateH1("longProfile", "Longitudinal profile;t (ID);#LTE#GT (MeV)", numLong, 0., numLong);
  analysisManager->CreateH1("transProfile", "Transverse profile;r (ID);#LTE#GT (MeV)", numTrans, 0, numTrans);
  analysisManager->CreateH1("Time", "Simulation time; time (s);Normalised entries", 3064, 0, 30);
  analysisManager->CreateH1("EnergyRatio", "Ratio of deposited to primary energy;E_{dep} /  E_{MC};Normalised entries", 100, 0, 1);
  analysisManager->CreateH1("longFirstMoment", "Longitudinal first moment;#LT#lambda#GT (ID);Normalised entries", 1024, 0, maxLong);
  analysisManager->CreateH1("transFirstMoment", "Transverse first moment;#LTr#GT (ID);Normalised entries", 1024, 0, maxTrans/10.);
  analysisManager->CreateH1("longSecondMoment", "Longitudinal second moment;#LT#lambda^{2}#GT (ID^{2});Normalised entries", 1024, 0, pow(maxLong,2)/200);
  analysisManager->CreateH1("transSecondMoment", "Transverse second moment;#LTr^{2}#GT (ID^{2});Normalised entries", 1024, 0, pow(maxTrans/10.,2));

  // Creating ntuple
  analysisManager->CreateNtuple("events", "per event data");
  analysisManager->CreateNtupleDColumn("EnergyMC");
  analysisManager->CreateNtupleDColumn("EnergyCell", fEventAction->GetCalEdep());
  analysisManager->CreateNtupleIColumn("rhoCell", fEventAction->GetCalRho());
  analysisManager->CreateNtupleIColumn("phiCell", fEventAction->GetCalPhi());
  analysisManager->CreateNtupleIColumn("zCell", fEventAction->GetCalZ());
  analysisManager->CreateNtupleIColumn("SimType");
  analysisManager->CreateNtupleDColumn("SimTime");
  analysisManager->CreateNtupleDColumn("GflashParams", fEventAction->GetGflashParams());
  analysisManager->CreateNtupleDColumn("ShowerStart_x");
  analysisManager->CreateNtupleDColumn("ShowerStart_y");
  analysisManager->CreateNtupleDColumn("ShowerStart_z");
  analysisManager->FinishNtuple();
}

RunAction::~RunAction() {
  delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  std::string fileName = analysisManager->GetFileName();
  int iterFile = 0;
  std::string::size_type position = 0;
  while (std::filesystem::exists(fileName)) {
    if ( fileName.find(".root") != std::string::npos ) {
      position = fileName.find("_file");
      if ( position == std::string::npos ) {
        fileName.insert(fileName.size() - std::string(".root").size(), "_file"+std::to_string(iterFile+1));
      } else {
        fileName.replace(position + std::string("_file").length(), fileName.size() - std::string(".root").length() - position - std::string("_file").length(), std::to_string(iterFile+1));
      }
      iterFile ++;
    }
  }
  analysisManager->OpenFile(fileName);
  // if geometry has changed...
  fEventAction->UpdateParameters();
  CALLGRIND_START_INSTRUMENTATION;
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  CALLGRIND_STOP_INSTRUMENTATION;
  // print histogram statistics
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // save histograms & ntuple
  analysisManager->Write();
  analysisManager->CloseFile();
}
