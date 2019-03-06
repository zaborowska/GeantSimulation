#include "FTFP_BERT.hh"
#include "GdmlDetectorConstruction.h"
#include "FullSimActions.h"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"

int main(int argc, char** argv)
{
  // Instantiate G4UIExecutive if interactive mode
  G4UIExecutive* ui = nullptr;
  if ( argc == 2 ) {
    ui = new G4UIExecutive(argc, argv);
  }

   if (argc<2)
   {
      G4cout << G4endl;
      G4cout << "Error! Mandatory input macro not specified!" << G4endl;
      G4cout << G4endl;
      G4cout << "Usage: "<<argv[0]<<"\n\t\t<geometry gdml source : mandatory>"<<
        "\n\t\t<settings macro : optional (vis instead)>"<< G4endl;
      G4cout << G4endl;
      return -1;
   }
   G4MTRunManager * runManager = new G4MTRunManager;
   runManager->SetNumberOfThreads(4);
   G4VModularPhysicsList* physicsList = new FTFP_BERT();
   runManager->SetUserInitialization(physicsList);
   // Load geometry (from GDML)
   G4cout << "Geometry loaded from  file " << argv[1]<<G4endl;
   G4VUserDetectorConstruction* detector = new test::GdmlDetectorConstruction(argv[1]);
   runManager->SetUserInitialization(detector);
   runManager->SetUserInitialization( new sim::FullSimActions );
   runManager->Initialize();
   std::cout << "after initialize" << std::endl;   


  //----------------
  // Visualization:
  //----------------
  G4cout << "Instantiating Visualization Manager......." << G4endl;
  G4VisManager* visManager = new G4VisExecutive;
  visManager -> Initialize ();

  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if(ui)
    {
      //--------------------------
      // Define (G)UI
      //--------------------------
      UImanager->ApplyCommand("/control/execute ../init_vis.mac");
      UImanager->SetMacroSearchPath("..");
      ui->SessionStart();
      delete ui;
    }
  else
    {
      // UI

      G4String command = "/control/execute ";
      G4String fileName = argv[2];
      G4cout << "/control/execute "<<argv[2]<<G4endl;
      UImanager->ApplyCommand(command+fileName);
    }
  delete visManager;
  delete runManager;

  return 0;
}
