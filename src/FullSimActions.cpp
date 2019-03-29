#include "FullSimActions.h"
#include "PrimaryGeneratorAction.hh"
#include "SaveToFileEventAction.hh"
#include "RunAction.hh"
#include "TThread.h"
#include "Randomize.hh"
#include <ctime>

namespace sim {
FullSimActions::FullSimActions():
  G4VUserActionInitialization() {}

FullSimActions::~FullSimActions() {}

void FullSimActions::BuildForMaster() const
{
  // if merging of files enabled
  // SaveToFileEventAction* eventAction = new SaveToFileEventAction;
  // SetUserAction(new RunAction(eventAction));
}

void FullSimActions::Build() const {
  SetUserAction(new PrimaryGeneratorAction());
  auto eventAction = new SaveToFileEventAction(25);
  SetUserAction(eventAction);
  SetUserAction(new RunAction(eventAction));
}
}
