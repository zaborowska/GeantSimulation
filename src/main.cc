#include "FTFP_BERT.hh"
#include "G4HadronicProcessStore.hh"
#include "DetectorConstruction.hh"
#include "ParallelWorldForReadout.h"
#include "G4ParallelWorldPhysics.hh"
#include "FullSimActions.h"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "myG4FastSimulationPhysics.hh"

int main(int argc, char** argv)
{
  // Instantiate G4UIExecutive if interactive mode
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }
#ifdef G4MULTITHREADED
   G4MTRunManager* runManager = new G4MTRunManager;
   runManager->SetNumberOfThreads(1);
#else
   G4RunManager* runManager = new G4RunManager;
#endif
   G4String parallelWorldName = "readoutWorld";

   DetectorConstruction* detector = new DetectorConstruction();
   detector->RegisterParallelWorld(new ParallelWorldForReadout(parallelWorldName, detector));
   runManager->SetUserInitialization(detector);

   G4VModularPhysicsList* physicsList = new FTFP_BERT();

   physicsList->RegisterPhysics(new G4ParallelWorldPhysics(parallelWorldName));

   // FASTSIM
   auto fastSimulationPhysics = new myG4FastSimulationPhysics();
   fastSimulationPhysics->BeVerbose();
   fastSimulationPhysics->ActivateFastSimulation("e-", parallelWorldName);
   fastSimulationPhysics->ActivateFastSimulation("e+", parallelWorldName);
   physicsList->RegisterPhysics( fastSimulationPhysics );
   // FASTSIM

   runManager->SetUserInitialization(physicsList);
   G4HadronicProcessStore::Instance()->SetVerbose(0);
   runManager->SetUserInitialization( new sim::FullSimActions(detector) );

  //----------------
  // Visualization:
  //----------------
  G4cout << "Instantiating Visualization Manager......." << G4endl;
  G4VisManager* visManager = new G4VisExecutive();
  visManager -> SetVerboseLevel (0);
  visManager -> Initialize ();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if(ui) {
    //--------------------------
    // Define (G)UI
    //--------------------------
    UImanager->ApplyCommand("/control/execute ../init_vis.mac");
    UImanager->SetMacroSearchPath("..");
    ui->SessionStart();
    delete ui;
  } else {
    // UI
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    G4cout << "/control/execute "<<argv[1]<<G4endl;
    UImanager->ApplyCommand(command+fileName);
  }
  delete visManager;
  delete runManager;

  return 0;
}
