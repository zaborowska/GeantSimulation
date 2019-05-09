#include <iostream>
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
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
  auto enMC = new TH1F("enMC", "MC energy (GeV);E_{MC} (GeV); Normalised entries", 1000, 0, 1.2 * maxEnergy);
  auto enTotal = new TH1F("enTotal", "total deposited energy (GeV);E_{dep} (GeV); Normalised entries", 1000, 0, 1.2 * maxEnergy);
  auto enFractionTotal = new TH1F("enFractionTotal", "fraction of deposited energy;E_{dep} /  E_{MC}; Normalised entries", 1000, 0, 1);
  auto numCells = new TH1F("numCells", "number of cells per event;## cells; Normalised entries", 100, 0, pow(netSize,3) );
  auto enCell = new TH1F("enCell", "cell energy distribution;E_{cell} (MeV); Normalised entries", 1000, 0, 1.2 * maxEnergy);
  auto longProfile = new TH1F("longProfile", "longitudinal profilee;t (layer);#LTE#GT (MeV)", netSize, -0.5, netSize - 0.5);
  auto transProfile = new TH1F("transProfile", "transverse profile;r (layer);#LTE#GT (MeV)", netMidCell, - 0.5, netMidCell - 0.5);
  auto enFractionCell = new TH1F("enFractionCell", "cell energy fraction distribution;E_{cell}/E_{MC}; Normalised entries", 1000, 0, 1);
  auto longFirstMoment = new TH1F("longFirstMoment", "longitudinal first moment;#LT#lambda#GT;Normalised entries", netSize*cellSizeMm, 0, netSize * cellSizeMm);
  auto longSecondMoment = new TH1F("longSecondMoment", "longitudinal second moment;#LT#lambda^{2}#GT;Normalised entries", netSize*cellSizeMm, 0, pow(netSize * cellSizeMm, 2));
  auto transFirstMoment = new TH1F("transFirstMoment", "transverse first moment;#LT#lambda#GT;Normalised entries", netSize*cellSizeMm, 0, netSize * cellSizeMm / scaleFactorProfile );
  auto transSecondMoment = new TH1F("transSecondMoment", "transverse second moment;#LT#lambda^{2}#GT;Normalised entries", netSize*cellSizeMm, 0, pow(netSize * cellSizeMm, 2) / pow(scaleFactorProfile, 2));
  auto enLayers = new TH2F("enLayers", "energy distribution per layer;E_{cell} (MeV); layer; Entries", 100, 0, 1000, netSize, -0.5, netSize - 0.5);
  auto enFractionLayers = new TH2F("enFractionLayers", "energy fraction distribution per layer;E_{cell}/E_{MC}; layer; Entries", 1000, 0, 1., netSize, -0.5, netSize - 0.5);
  auto transProfileLayers = new TH2F("transProfileLayers", "transverse profile per layer ;r (layer); layer;#LTE#GT (MeV)", netMidCell, - 0.5, netMidCell - 0.5, netSize, -0.5, netSize - 0.5);

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
      enLayers->Fill(eCell, tDistance);
      longProfile->Fill(tDistance, eCell);
      transProfile->Fill(rDistance, eCell);
      transProfileLayers->Fill(rDistance, tDistance, eCell);
      enFractionCell->Fill(eCellFraction);
      enFractionLayers->Fill(eCellFraction, tDistance);
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
  enLayers->Scale(1./iterEvents);
  enFractionLayers->Scale(1./iterEvents);
  transProfileLayers->Scale(1./iterEvents);
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
  enLayers->Write();
  enFractionLayers->Write();
  transProfileLayers->Write();
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
