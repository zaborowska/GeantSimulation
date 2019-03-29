#ifndef SaveToFileEventAction_h
#define SaveToFileEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class SaveToFileEventAction : public G4UserEventAction
{
public:
  SaveToFileEventAction();
  SaveToFileEventAction(G4int);
  virtual ~SaveToFileEventAction() {};

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);
  std::vector<G4double>& GetCalEdep() { return fCalEdep; }
  std::vector<G4int>& GetCalX() { return fCalX; }
  std::vector<G4int>& GetCalY() { return fCalY; }
  std::vector<G4int>& GetCalZ() { return fCalZ; }

private:
  G4int fHID;
  G4int fCellNo;
  std::vector<G4double> fCalEdep;
  std::vector<G4int> fCalX;
  std::vector<G4int> fCalY;
  std::vector<G4int> fCalZ;
};

#endif
