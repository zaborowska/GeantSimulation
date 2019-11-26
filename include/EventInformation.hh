#ifndef EVENTINFORMATION_H
#define EVENTINFORMATION_H

#include "G4VUserEventInformation.hh"
#include "globals.hh"
#include <iostream>
#include <vector>

class EventInformation : public G4VUserEventInformation
{
public:
  EventInformation();
  virtual ~EventInformation();
  inline virtual void Print() const;
  enum eSimType {eFullSim = 0, eGflash = 1, eML = 2};
  void SetSimType(eSimType);
  eSimType GetSimType() const;
  inline std::vector<G4double>& GetGflashParams() { return fGflashParams; }
  void SetGflashParams (std::vector<G4double>& aGflashParams);
private:
  eSimType fSimType;
  // parameters used for the current (single-particle) event
  std::vector<G4double> fGflashParams;
};

#endif
