#include <iostream>
#include "TH1.h"
#include "TH1F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TFitResult.h"
#include "ROOT/RDataFrame.hxx"

void validationPlots(const std::string& aInput, const std::string& aOutput) {
  TFile f(aInput.c_str(), "READ");

  // Set initial parameters
  const int netSize = 25;
  const double cellSizeMm = 10.;
  const double scaleFactorProfile = 10.;
  const int netMidCell = floor (netSize / 2);
  double maxEnergy = 500;

  // Check if flat energy spectrum or single-energy simulation is analysed
  ROOT::RDataFrame d("events", &f, {"EnergyMC"});
  auto mc_histo =  d.Histo1D("EnergyMC");
  double energySpan = mc_histo->GetXaxis()->GetXmax() - mc_histo->GetXaxis()->GetXmin();
  if (energySpan < 1e3) {
    maxEnergy = mc_histo->GetMean() / 1.e3; // unit converted to GeV
    std::cout << std::endl << "Detected single-energy simulation, particle energy: " << maxEnergy << std::endl << std::endl;
  }
  auto mesh = new TH3F("mesh", "mesh", netSize, -0.5, netSize - 0.5, netSize, -0.5, netSize - 0.5, netSize, -0.5, netSize - 0.5);
  mesh->SetTitle(";z;x;y");
  auto enMC = new TH1D("enMC", "MC energy (GeV)", 1000, 0, 1.2 * maxEnergy);
  enMC->SetTitle("MC energy;E_{MC} (GeV); Normalised entries");
  auto enTotal = new TH1D("enTotal", "total energy (GeV)", 1000, 0, 1.2 * maxEnergy);
  enTotal->SetTitle("total deposited energy;E_{dep} (GeV); Normalised entries");
  auto enFractionTotal = new TH1D("enFractionTotal", "fraction of deposited energy", 1000, 0, 1);
  enFractionTotal->SetTitle("fraction of deposited energy;E_{dep} /  E_{MC}; Normalised entries");
  auto numCells = new TH1D("numCells", "number of cells per event", 100, 0, pow(netSize,3) );
  numCells->SetTitle("number of cells per event;## cells; Normalised entries");
  auto enCell = new TH1D("enCell", "cell energy distribution", 1000, 0, 1.2 * maxEnergy);
  enCell->SetTitle("cell energy distribution;E_{cell} (MeV); Normalised entries");
  auto longProfile = new TH1D("longProfile", "longitudinal profile", netSize, -0.5, netSize - 0.5);
  longProfile->SetTitle("longitudinal profile;t (layer);#LTE#GT (MeV)");
  auto transProfile = new TH1D("transProfile", "transverse profile", netMidCell, - 0.5, netMidCell - 0.5);
  transProfile->SetTitle("transverse profile;r (layer);#LTE#GT (MeV)");
  auto enFractionCell = new TH1D("enFractionCell", "cell energy fraction distribution", 1000, 0, 1);
  enFractionCell->SetTitle("cell energy fraction distribution;E_{cell}/E_{MC}; Normalised entries");
  auto longFirstMoment = new TH1D("longFirstMoment", "longitudinal first moment", netSize*cellSizeMm, 0, netSize * cellSizeMm);
  longFirstMoment->SetTitle("longitudinal first moment;#LT#lambda#GT;Normalised entries");
  auto longSecondMoment = new TH1D("longSecondMoment", "longitudinal second moment", netSize*cellSizeMm, 0, pow(netSize * cellSizeMm, 2));
  longSecondMoment->SetTitle("longitudinal second moment;#LT#lambda^{2}#GT;Normalised entries");
  auto transFirstMoment = new TH1D("transFirstMoment", "transverse first moment", netSize*cellSizeMm, 0, netSize * cellSizeMm / scaleFactorProfile );
  transFirstMoment->SetTitle("transverse first moment;#LT#lambda#GT;Normalised entries");
  auto transSecondMoment = new TH1D("transSecondMoment", "transverse second moment", netSize*cellSizeMm, 0, pow(netSize * cellSizeMm, 2) / pow(scaleFactorProfile, 2));
  transSecondMoment->SetTitle("transverse second moment;#LT#lambda^{2}#GT;Normalised entries");

  TH1D* enLayers[netSize];
  TH1D* enFractionLayers[netSize];
  TH1D* transProfileLayers[netSize];
  for(uint iLayer = 0; iLayer < netSize; ++iLayer) {
    enLayers[iLayer] = new TH1D(("enLayer"+std::to_string(iLayer)).c_str(), ("energy distribution for layer "+std::to_string(iLayer)).c_str(), 100, 0, 1000);
    enLayers[iLayer]->SetTitle(("energy distribution for layer " +std::to_string(iLayer)+";E_{cell} (MeV); Entries").c_str());
    enFractionLayers[iLayer] = new TH1D(("enFractionLayer"+std::to_string(iLayer)).c_str(), ("energy fraction distribution for layer "+std::to_string(iLayer)).c_str(), 1000, 0, 1);
    enFractionLayers[iLayer]->SetTitle(("energy fraction distribution for layer " +std::to_string(iLayer)+";E_{cell}/E_{MC}; Entries").c_str());
    transProfileLayers[iLayer] = new TH1D(("transProfileLayer"+std::to_string(iLayer)).c_str(), ("transverse profile for layer "+std::to_string(iLayer)).c_str(), netMidCell, - 0.5, netMidCell - 0.5);
    transProfileLayers[iLayer]->SetTitle(("transverse profile for layer " +std::to_string(iLayer)+";r (layer);#LTE#GT (MeV)").c_str());
  }

  TTree* events = nullptr;
  TTreeReader eventsReader("events",&f);
  TTreeReaderValue<double> energyMC(eventsReader, "EnergyMC");
  TTreeReaderValue<std::vector<double>> energyCellV(eventsReader, "EnergyCell");
  TTreeReaderValue<std::vector<int>> xCellV(eventsReader, "xCell");
  TTreeReaderValue<std::vector<int>> yCellV(eventsReader, "yCell");
  TTreeReaderValue<std::vector<int>> zCellV(eventsReader, "zCell");

  uint iterEvents = 0;
  // retireved from input
  size_t eventSize = 0;
  uint xCell = 0, yCell = 0, zCell = 0;
  double eCell = 0;
  // calculated
  uint tDistance = 0;
  double rDistance = 0, eCellFraction = 0, sumEnergyDeposited = 0;
  double tFirstMoment = 0, tSecondMoment = 0, rFirstMoment = 0, rSecondMoment = 0;
  while(eventsReader.Next()){
    // std::cout << "event: " << iterEvents << "\t" << *energyMC << std::endl;
    sumEnergyDeposited = 0;
    eventSize = energyCellV->size();
    tFirstMoment = 0;
    tSecondMoment = 0;
    rFirstMoment = 0;
    rSecondMoment = 0;
    for (uint iEntry = 0; iEntry < eventSize; ++iEntry) {
      // get data (missing: angle at entrance)
      xCell = xCellV->at(iEntry);
      yCell = yCellV->at(iEntry);
      zCell = zCellV->at(iEntry);
      eCell = energyCellV->at(iEntry);
      eCellFraction = eCell / *energyMC;
      // make calculations
      tDistance = zCell; // assumption: particle enters calorimeter perpendiculary
      rDistance = sqrt( (xCell - netMidCell) * (xCell - netMidCell) + (yCell - netMidCell) * (yCell - netMidCell));
      tFirstMoment += eCell * (tDistance + 0.5) * cellSizeMm;
      tSecondMoment += eCell * pow((tDistance + 0.5) * cellSizeMm, 2);
      rFirstMoment += eCell * rDistance * cellSizeMm;
      rSecondMoment += eCell * pow(rDistance * cellSizeMm, 2);
      // fill histograms
      mesh->Fill(zCell, xCell, yCell, eCell);
      enCell->Fill(eCell);
      enLayers[tDistance]->Fill(eCell);
      longProfile->Fill(tDistance, eCell);
      transProfile->Fill(rDistance, eCell);
      transProfileLayers[tDistance]->Fill(rDistance, eCell);
      enFractionCell->Fill(eCellFraction);
      enFractionLayers[tDistance]->Fill(eCellFraction);
      sumEnergyDeposited += eCell;
    }
    enMC->Fill(*(energyMC) / 1.e3);  // convert to GeV
    enTotal->Fill(sumEnergyDeposited / 1.e3);  // convert to GeV
    enFractionTotal->Fill(sumEnergyDeposited / (*energyMC));  // convert to GeV
    numCells->Fill(eventSize);
    tFirstMoment /= sumEnergyDeposited;
    tSecondMoment /= sumEnergyDeposited;
    rFirstMoment /= sumEnergyDeposited;
    rSecondMoment /= sumEnergyDeposited;
    longFirstMoment->Fill(tFirstMoment);
    longSecondMoment->Fill(tSecondMoment);
    transFirstMoment->Fill(rFirstMoment);
    transSecondMoment->Fill(rSecondMoment);
    iterEvents++;
  }
  // Normalize
  enMC->Scale(1./iterEvents);
  enTotal->Scale(1./iterEvents);
  enFractionTotal->Scale(1./iterEvents);
  numCells->Scale(1./iterEvents);
  enCell->Scale(1./iterEvents);
  longProfile->Scale(1./iterEvents);
  transProfile->Scale(1./iterEvents);
  mesh->Scale(1./iterEvents);
  enFractionCell->Scale(1./iterEvents);
  longFirstMoment->Scale(1./iterEvents);
  longSecondMoment->Scale(1./iterEvents);
  transFirstMoment->Scale(1./iterEvents);
  transSecondMoment->Scale(1./iterEvents);
  for(uint iLayer = 0; iLayer < netSize; ++iLayer) {
    enLayers[iLayer]->Scale(1./iterEvents);
    enFractionLayers[iLayer]->Scale(1./iterEvents);
    transProfileLayers[iLayer]->Scale(1./iterEvents);
  }
  // Store histograms
  std::cout << "Saving output histograms to \"" << aOutput << "\"" << std::endl;
  TFile out(aOutput.c_str(), "RECREATE");
  out.cd();
  mesh->Write();
  enMC->Write();
  enTotal->Write();
  enFractionTotal->Write();
  numCells->Write();
  enCell->Write();
  longProfile->Write();
  transProfile->Write();
  enFractionCell->Write();
  longFirstMoment->Write();
  longSecondMoment->Write();
  transFirstMoment->Write();
  transSecondMoment->Write();
  for(uint iLayer = 0; iLayer < netSize; ++iLayer) {
    enLayers[iLayer]->Write();
    enFractionLayers[iLayer]->Write();
    transProfileLayers[iLayer]->Write();
  }
  out.Close();
  f.Close();
return;
}

int main(int argc, char** argv){
  if (argc < 2) {
    std::cout << "Please specify path to the input file." << std::endl;
    return -1;
  }
  std::string outputName = "";
  if (argc < 3) {
    std::string inputName = argv[1];
    outputName = "validation_" + inputName.substr(inputName.find_last_of("/") + 1,
                                  - 1);
    std::cout << "Using default output path: \"./" << outputName << "\"" << std::endl;
  } else {
    outputName = argv[2];
  }
  validationPlots(argv[1], outputName);
  return 0;
}
