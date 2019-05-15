#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

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
  analysisManager->FinishNtuple();
}

RunAction::~RunAction() {
  delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {
  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  G4String fileName = "ntuples_"+std::to_string(std::time(NULL));
  analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // save histograms & ntuple
  analysisManager->Write();
  analysisManager->CloseFile();
}
