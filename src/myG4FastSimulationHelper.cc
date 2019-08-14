//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
#include "myG4FastSimulationHelper.hh"

#include "G4ProcessManager.hh"
#include "G4FastSimulationManagerProcess.hh"

void myG4FastSimulationHelper::ActivateFastSimulation(G4ProcessManager* pmanager)
{
  G4FastSimulationManagerProcess* fastSimProcess = new G4FastSimulationManagerProcess("fastSimProcess_massGeom");
  // -- For the mass geometry case, the G4FastSimulationManagerProcess
  // -- is a PostStep process, and ordering does not matter:
  pmanager-> AddDiscreteProcess(fastSimProcess);
  // even though the parametrisation happens in the mass geometry, it is possible
  // that parallel world exists (with parallel world physics). In that case
  // make sure fast simulation is the first process to be checked before checking
  // the parallel world for sensitive detectors
  pmanager->SetProcessOrderingToLast(fastSimProcess, idxPostStep);
}

void myG4FastSimulationHelper::ActivateFastSimulation(G4ProcessManager* pmanager, G4String parallelGeometryName )
{
  G4FastSimulationManagerProcess* fastSimProcess = new G4FastSimulationManagerProcess("fastSimProcess_parallelGeom",
										      parallelGeometryName);
  // -- For the parallel geometry case, the G4FastSimulationManagerProcess
  // -- is an Along+PostStep process, and ordering matters:
  pmanager->AddProcess(fastSimProcess);
  pmanager->SetProcessOrdering(fastSimProcess, idxAlongStep, 1);
  // registered as the process with highest order so it is invoked as the first one
  // and it is exclusively forced (and the only one executed)
  pmanager->SetProcessOrderingToLast(fastSimProcess, idxPostStep);
}
