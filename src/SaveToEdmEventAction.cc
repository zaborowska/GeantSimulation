// #include "SaveToEdmEventAction.hh"
// #include "G4Event.hh"
// #include "G4RunManager.hh"
// #include "G4EventManager.hh"
// #include "G4HCofThisEvent.hh"
// #include "G4VHitsCollection.hh"
// #include "G4SDManager.hh"
// #include "G4SystemOfUnits.hh"

// // from FCCSW
// #include "CalorimeterHit.h"


// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// SaveToEdmEventAction::SaveToEdmEventAction()
// : G4UserEventAction(),
//   fHID(-1),
//   fCellNo(201),
//   fOutputName("Standalone.root"),
//   store(),
//   writer(fOutputName, &store),
//   hitscoll(store.create<fcc::SimCaloHitCollection>("hits")),
//   clusterscoll(store.create<fcc::SimCaloClusterCollection>("clusters"))
// {
//   // set printing per each event
//   G4RunManager::GetRunManager()->SetPrintProgress(10);
//   fHadCalEdep.resize(fCellNo*fCellNo*fCellNo, 0.);
//   writer.registerForWrite<fcc::SimCaloHitCollection>("hits");
//   writer.registerForWrite<fcc::SimCaloClusterCollection>("clusters");
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// SaveToEdmEventAction::SaveToEdmEventAction(G4int aCellNo)
//   : G4UserEventAction(),
//     fHID(-1),
//     fCellNo(aCellNo),
//     fOutputName("Standalone.root"),
//     store(),
//     writer(fOutputName, &store),
//     hitscoll(store.create<fcc::SimCaloHitCollection>("hits")),
//     clusterscoll(store.create<fcc::SimCaloClusterCollection>("clusters"))
// {
//   // set printing per each event
//   G4RunManager::GetRunManager()->SetPrintProgress(10);
//   fHadCalEdep.resize(fCellNo*fCellNo*fCellNo, 0.);
//   G4cout<<"start writing"<<G4endl;
//   writer.registerForWrite<fcc::SimCaloHitCollection>("hits");
//   writer.registerForWrite<fcc::SimCaloClusterCollection>("clusters");
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// SaveToEdmEventAction::SaveToEdmEventAction(G4int aCellNo, G4String aOut)
// : G4UserEventAction(),
//   fHID(-1),
//   fCellNo(aCellNo),
//   fOutputName(aOut),
//   store(),
//   writer(fOutputName, &store),
//     hitscoll(store.create<fcc::SimCaloHitCollection>("hits")),
//     clusterscoll(store.create<fcc::SimCaloClusterCollection>("clusters"))
// {
//   // set printing per each event
//   G4RunManager::GetRunManager()->SetPrintProgress(10);
//   fHadCalEdep.resize(fCellNo*fCellNo*fCellNo, 0.);
//   writer.registerForWrite<fcc::SimCaloHitCollection>("hits");
//   writer.registerForWrite<fcc::SimCaloClusterCollection>("clusters");
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// SaveToEdmEventAction::~SaveToEdmEventAction()
// {
//   G4cout<<"finish writing"<<G4endl;
//     writer.finish();
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void SaveToEdmEventAction::BeginOfEventAction(const G4Event*)
// {
//     if (fHID==-1) {
//       G4SDManager* sdManager = G4SDManager::GetSDMpointer();
//       fHID = sdManager->GetCollectionID("ECalorimeterColl");
//     }

// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void SaveToEdmEventAction::EndOfEventAction(const G4Event* event)
// {
//     G4HCofThisEvent* hce = event->GetHCofThisEvent();
//     if (!hce)
//     {
//         G4ExceptionDescription msg;
//         msg << "No hits collection of this event found." << G4endl;
//         G4Exception("SaveToEdmEventAction::EndOfEventAction()",
//                     "B5Code001", JustWarning, msg);
//         return;
//     }

//     // Get hits collections

//     test::CalorimeterHitsCollection* hcHC
//       = static_cast<test::CalorimeterHitsCollection*>(hce->GetHC(fHID));

//     if ( (!hcHC) )
//     {
//         G4ExceptionDescription msg;
//         msg << "Some of hits collections of this event not found." << G4endl;
//         G4Exception("SaveToEdmEventAction::EndOfEventAction()",
//                     "B5Code001", JustWarning, msg);
//         return;
//     }

//     //
//     // Fill histograms & ntuple
//     //
//     // auto& hitsclusterscoll = store.create<fcc::SimCaloClusterHitAssociationCollection>("hitsclusters");
//     // writer.registerForWrite<fcc::SimCaloClusterHitAssociationCollection>("hitsclusters");

//     // Fill ntuple
//     G4int totalHadHit = 0;
//     G4double totalHadE = 0.;
//     for (G4int i=0;i<fCellNo*fCellNo*fCellNo;i++)
//     {
//       test::CalorimeterHit* hit = (*hcHC)[i];
//       G4double eDep = hit->GetEdep();
//       if (eDep>0.)
//       {
//         totalHadHit++;
//         totalHadE += eDep;
//       }
//       fHadCalEdep[i] = eDep;
//       if(hit->GetXid() != -1 && hit->GetYid() != -1 && hit->GetZid() != -1) {
//         //hit->Print();
//         auto edmhit = fcc::SimCaloHit();
//         edmhit.Core().Energy = eDep/GeV;
//         edmhit.Core().Cellid = fCellNo*fCellNo*hit->GetXid()+fCellNo*hit->GetYid()+hit->GetZid();
//         //G4cout<<"Storing hit "<<eDep<<" "<<fCellNo*fCellNo*hit->GetXid()+fCellNo*hit->GetYid()+hit->GetZid()<<G4endl;
//         auto edmcluster = fcc::SimCaloCluster();
//         edmcluster.Core().Energy = eDep/GeV;
//         auto& pos = edmcluster.Core().position;
//         pos.X = hit->GetPos().x()/mm;
//         pos.Y = hit->GetPos().y()/mm;
//         pos.Z = hit->GetPos().z()/mm;
//         // auto edmhc = fcc::SimCaloClusterHitAssociation();
//         // edmhc.Cluster(edmcluster);
//         // edmhc.Hit(edmhit);
//         hitscoll.push_back(edmhit);
//         clusterscoll.push_back(edmcluster);
//         // hitsclusterscoll.push_back(edmhc);
//       }
//     }
//     G4cout<<" __write current event..."<<G4endl;
//     writer.writeEvent();
//     store.clearCollections();

//     // Print diagnostics
//     G4int printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
//     if ( printModulo==0 || event->GetEventID() % printModulo != 0) return;

//     G4PrimaryParticle* primary = event->GetPrimaryVertex(0)->GetPrimary(0);
//     G4cout << G4endl
//            << ">>> Event " << event->GetEventID() << " >>> Simulation truth : "
//            << primary->GetG4code()->GetParticleName()
//            << " " << primary->GetMomentum() << G4endl;

//     // Had calorimeter
//     G4cout << "Hadron Calorimeter has " << totalHadHit << " hits. Total Edep is "
//            << totalHadE/MeV << " (MeV)" << G4endl;

// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
