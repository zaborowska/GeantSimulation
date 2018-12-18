#include "FullSimActions.h"
#include "B1PrimaryGeneratorAction.hh"
// #include "SaveToEdmEventAction.hh"

namespace sim {
FullSimActions::FullSimActions():
  G4VUserActionInitialization() {}

FullSimActions::~FullSimActions() {}

void FullSimActions::Build() const {
  SetUserAction(new B1PrimaryGeneratorAction());
  // SetUserAction(new SaveToEdmEventAction(201));
}
}
