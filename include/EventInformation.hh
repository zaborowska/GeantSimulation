#ifndef EVENTINFORMATION_H
#define EVENTINFORMATION_H

#include "G4VUserEventInformation.hh"
#include "globals.hh"
#include <iostream>
#include <vector>
#include "G4ThreeVector.hh"

class EventInformation : public G4VUserEventInformation
{
public:
  EventInformation();
  virtual ~EventInformation();
  inline virtual void Print() const;
  enum eSimType {eFullSim = 0, eGflash = 1, eML = 2};
  void SetSimType(eSimType);
  eSimType GetSimType() const;
  void SetShowerStart(G4ThreeVector);
  G4ThreeVector GetShowerStart() const;
  G4bool IfShowerStarted() const;
  inline std::vector<G4double>& GetGflashParams() { return fGflashParams; }
  void SetGflashParams (std::vector<G4double>& aGflashParams);
private:
  eSimType fSimType;
  G4ThreeVector fPosition;
  G4bool fShowerStarted;
  // parameters used for the current (single-particle) event
  std::vector<G4double> fGflashParams;
};

#endif
