#include "FullSimActions.h"
#include "PrimaryGeneratorAction.hh"
#include "SaveToFileEventAction.hh"
#include "RunAction.hh"
#include "Randomize.hh"
#include "DetectorConstruction.hh"
#include <ctime>

namespace sim {
FullSimActions::FullSimActions():
  G4VUserActionInitialization(), fDetector(nullptr) {}

FullSimActions::FullSimActions(const DetectorConstruction* aDetector):
  G4VUserActionInitialization(), fDetector(aDetector) {}

FullSimActions::~FullSimActions() {}

void FullSimActions::BuildForMaster() const
{
  // if merging of files enabled
  // SaveToFileEventAction* eventAction = new SaveToFileEventAction;
  // SetUserAction(new RunAction(eventAction));
}

void FullSimActions::Build() const {
  SetUserAction(new PrimaryGeneratorAction());
  auto eventAction = new SaveToFileEventAction(fDetector);
  SetUserAction(eventAction);
  SetUserAction(new RunAction(eventAction));
}
}
