#ifndef DEBUGKILLPROCESSMESSENGER_H
#define DEBUGKILLPROCESSMESSENGER_H

#include "G4UImessenger.hh"
#include "globals.hh"

class DebugKillProcess;
class G4UIcmdWithADoubleAndUnit;

class DebugKillProcessMessenger: public G4UImessenger
{
  public:
    DebugKillProcessMessenger(DebugKillProcess*);
   ~DebugKillProcessMessenger();
    virtual void SetNewValue(G4UIcommand*, G4String);
  private:
    DebugKillProcess* fDebugKillProcess;
    G4UIcmdWithADoubleAndUnit* fEnergyThresholdCmd;
};

#endif
