#include "FTFP_BERT.hh"
#include "G4UnitsTable.hh"

// directly taken from FCCSW
#include "GdmlDetectorConstruction.h"
#include "FullSimActions.h"

#include <G4MTRunManager.hh>

#include "G4UImanager.hh"
#include "G4RunManager.hh"
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc, char** argv)
{
   if (argc<3)
   {
      G4cout << G4endl;
      G4cout << "Error! Mandatory input macro not specified!" << G4endl;
      G4cout << G4endl;
      G4cout << "Usage: "<<argv[0]<<"\n\t\t<geometry gdml source : mandatory>"<<
        "\n\t\t<settings macro : mandatory>"<< G4endl;
      G4cout << G4endl;
      return -1;
   }
   G4MTRunManager * runManager = new G4MTRunManager;
   runManager->SetNumberOfThreads(1);
   G4VModularPhysicsList* physicsList = new FTFP_BERT();
   runManager->SetUserInitialization(physicsList);
   // Load geometry (from GDML)
   G4cout << "Geometry loaded from  file " << argv[1]<<G4endl;
   G4VUserDetectorConstruction* detector = new test::GdmlDetectorConstruction(argv[1]);
   runManager->SetUserInitialization(detector);
   runManager->SetUserInitialization( new sim::FullSimActions );
   runManager->Initialize();
   std::cout << "after initialize" << std::endl;   
   // UI
   G4UImanager* UImanager = G4UImanager::GetUIpointer();
   G4String command = "/control/execute ";
   G4String fileName = argv[2];
   G4cout << "/control/execute "<<argv[2]<<G4endl;
   UImanager->ApplyCommand(command+fileName);

   // Free the store: user actions, physics_list and detector_description are
   //                 owned and deleted by the run manager, so they should not
   //                 be deleted in the main() program !

   delete runManager;

   return 0;
}
