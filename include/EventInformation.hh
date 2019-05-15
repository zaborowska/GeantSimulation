#ifndef EVENTINFORMATION_H
#define EVENTINFORMATION_H

#include "G4VUserEventInformation.hh"
#include <iostream>

class EventInformation : public G4VUserEventInformation
{
public:
  EventInformation();
  virtual ~EventInformation();
  inline virtual void Print() const;
  enum eSimType {eFullSim = 0, eGflash = 1, eML = 2};
  void SetSimType(eSimType);
  eSimType GetSimType() const;
private:
  eSimType fSimType;
};

#endif
