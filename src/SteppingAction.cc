#include "SteppingAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "EventInformation.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh" 

SteppingAction::SteppingAction() : G4UserSteppingAction() {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *step)
{
  // get first interaction position for the primary particle
  if (step->GetTrack()->GetParentID() == 0) {
    auto info = static_cast<EventInformation*>(G4EventManager::GetEventManager()->GetUserInformation());
    if (info->IfShowerStarted()) return;
    auto processType = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType();
    if (processType != fTransportation && processType != fParallel)
    {
      info->SetShowerStart(step->GetPostStepPoint()->GetPosition());
    }
  }
}
