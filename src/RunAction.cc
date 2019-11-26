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
  // analysisManager->SetNtupleMerging(true);
  analysisManager->SetVerboseLevel(1);

  // Creating ntuple
  analysisManager->CreateNtuple("events", "per event data");
  analysisManager->CreateNtupleDColumn("EnergyMC");
  analysisManager->CreateNtupleDColumn("EnergyCell", fEventAction->GetCalEdep());
  analysisManager->CreateNtupleIColumn("xCell", fEventAction->GetCalX());
  analysisManager->CreateNtupleIColumn("yCell", fEventAction->GetCalY());
  analysisManager->CreateNtupleIColumn("zCell", fEventAction->GetCalZ());
  analysisManager->CreateNtupleIColumn("SimType");
  analysisManager->CreateNtupleDColumn("SimTime");
  analysisManager->CreateNtupleDColumn("GflashParams", fEventAction->GetGflashParams());
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
