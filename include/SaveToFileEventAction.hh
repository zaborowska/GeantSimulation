#ifndef SaveToFileEventAction_h
#define SaveToFileEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include "TFile.h"
#include "TTree.h"


class SaveToFileEventAction : public G4UserEventAction
{
public:
  SaveToFileEventAction();
  SaveToFileEventAction(TFile*, G4int);
  virtual ~SaveToFileEventAction() {};

  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

private:
  G4int fHID;
  G4int fCellNo;
  TFile* fOutput;
  TTree* t2;
};

#endif
