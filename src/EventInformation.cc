#include "EventInformation.hh"
#include "globals.hh"

EventInformation::EventInformation(): fSimType(eFullSim) {}
EventInformation::~EventInformation() {}

void EventInformation::SetSimType(EventInformation::eSimType aSimType) {
  fSimType = aSimType;
}

EventInformation::eSimType EventInformation::GetSimType() const {
  return fSimType;
}

void EventInformation::Print() const {
  if (fSimType == ::EventInformation::eSimType::eFullSim)
    G4cout << "Event simulated with full sim." << G4endl;
  else
    G4cout << "Event simulated with fast sim." << G4endl;
}
