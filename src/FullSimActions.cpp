#include "FullSimActions.h"
#include "PrimaryGeneratorAction.hh"
#include "SaveToFileEventAction.hh"
#include "TThread.h"

namespace sim {
FullSimActions::FullSimActions():
  G4VUserActionInitialization() {
  std::cout << "INITIALIZE FULL SIM ACTIONS" << std::endl;
  TThread::Initialize();
  f = new TFile("trialWithcut32Window.root","recreate");
}

FullSimActions::~FullSimActions()
{
  f->Write();
  f->Close();
  delete f;
}

void FullSimActions::Build() const {
  SetUserAction(new PrimaryGeneratorAction());
  SetUserAction(new SaveToFileEventAction(f, 32));
  std::cout << "BUILD ACTIONS" << std::endl;
}
}
