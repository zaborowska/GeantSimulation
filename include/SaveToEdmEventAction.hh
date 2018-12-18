// #ifndef SaveToEdmEventAction_h
// #define SaveToEdmEventAction_h 1

// #include "G4UserEventAction.hh"
// #include "globals.hh"
// #include <vector>
// // podio specific includes
// //#include "podio/EventStore.h"
// //#include "podio/ROOTWriter.h"
// // Data model
// // #include "datamodel/SimCaloHit.h"
// // #include "datamodel/SimCaloHitCollection.h"
// // #include "datamodel/SimCaloCluster.h"
// // #include "datamodel/SimCaloClusterCollection.h"
// // #include "datamodel/SimCaloClusterHitAssociation.h"
// // #include "datamodel/SimCaloClusterHitAssociationCollection.h"

// class SaveToEdmEventAction : public G4UserEventAction
// {
// public:
//   SaveToEdmEventAction();
//   SaveToEdmEventAction(G4int);
//   SaveToEdmEventAction(G4int,G4String);
//   virtual ~SaveToEdmEventAction();

//   virtual void BeginOfEventAction(const G4Event*);
//   virtual void EndOfEventAction(const G4Event*);

// private:
//   G4int fHID;
//   G4int fCellNo;
//   std::vector<G4double> fHadCalEdep;
//   G4String fOutputName;
//   //podio::EventStore store;
//   //podio::ROOTWriter writer;
//   fcc::SimCaloHitCollection& hitscoll;
//   fcc::SimCaloClusterCollection& clusterscoll;
// };

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// #endif
