#include <iostream>
#include "TH1.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TFitResult.h"
#include "ROOT/RDataFrame.hxx"
#include "createHistograms.h"

void createHistograms(const std::string& aInput, const std::string& aOutput, double aMinEnergyMC = 1, double aMaxEnergyMC = 500, float aCellUnitToMeV = 1.) {
  TFile f(aInput.c_str(), "READ");

  // Set initial parameters
  const int netSize = 25;
  const double cellSizeMm = 10.;
  const double scaleFactorProfile = 10.;
  const int netMidCell = floor (netSize / 2);
  double minEnergy = aMinEnergyMC;
  double maxEnergy = aMaxEnergyMC;
  bool eventSelection = false; // true if not all events are used for analysis

  // Check if flat energy spectrum or single-energy simulation is analysed
  ROOT::RDataFrame d("events", &f, {"EnergyMC","SimTime"});
  auto mc_histo =  d.Histo1D("EnergyMC");
  double energySpan = mc_histo->GetXaxis()->GetXmax() - mc_histo->GetXaxis()->GetXmin();
  if (energySpan < 1e3) {
    maxEnergy = mc_histo->GetMean() / 1.e3; // unit converted to GeV
    minEnergy = maxEnergy;
    std::cout << std::endl << "Detected single-energy simulation, particle energy: " << maxEnergy << " GeV." << std::endl << std::endl;
  } else {
    // chose only bin of a flat spectrum
    eventSelection = true;
    std::cout << std::endl << "Flat energy spectrum simulation, choosing particles with energy within: " << minEnergy << " GeV and " << maxEnergy << " GeV." << std::endl << std::endl;
  }
  // check if SimTime exists in ROOT file
  bool include_simtime = false, include_simtype = false;
  auto col_names = d.GetColumnNames();
  if (std::find(col_names.begin(), col_names.end(), "SimTime") != col_names.end()) {
      include_simtime = true;
  }
  if (std::find(col_names.begin(), col_names.end(), "SimType") != col_names.end()) {
      include_simtype = true;
  }
  TH1F *simType = nullptr, *simTime = nullptr;
  if (include_simtime) {
    auto time_histo =  d.Histo1D("SimTime");
    uint time_mean = 0;
    for(uint iBin = 1; iBin < time_histo->GetNbinsX(); ++iBin) {
      if (time_histo->GetBinContent(iBin) > time_histo->GetBinContent(time_mean)) {
        time_mean = iBin;
      }
    }
    TF1* fitgaus = new TF1("fitgaus","gaus", time_histo->GetBinLowEdge(time_mean-5), time_histo->GetBinLowEdge(time_mean + 5) );
    auto fitResTime = time_histo->Fit(fitgaus,"RqnS");
    double minTime = fitResTime->Parameter(1) - fitResTime->Parameter(2) * 5;
    double maxTime = fitResTime->Parameter(1) + fitResTime->Parameter(2) * 5;
    if (minTime < 0) minTime = 0;
    simTime = new TH1F("simTime", "simulation time (per event);simulation time (s); Normalised entries", 100, minTime, maxTime);
  }
  if (include_simtype) {
    simType = new TH1F("simType", "type of simulation;simulation type; Normalised entries", 3, -0.5, 2.5);
    simType->GetXaxis()->SetBinLabel(1,"full sim");
    simType->GetXaxis()->SetBinLabel(2,"GFlash");
    simType->GetXaxis()->SetBinLabel(3,"ML");
  }
  auto mesh = new TH3F("mesh", "mesh", netSize, -0.5, netSize - 0.5, netSize, -0.5, netSize - 0.5, netSize, -0.5, netSize - 0.5);
  mesh->SetTitle(";z;x;y");
  auto enMC = new TH1F("enMC", "MC energy (GeV);E_{MC} (GeV); Normalised entries", 1000, 0, floor(1.2 * maxEnergy));
  auto enTotal = new TH1F("enTotal", "total deposited energy (GeV);E_{dep} (GeV); Normalised entries", 1000, 0, floor(1.2 * maxEnergy));
  auto enFractionTotal = new TH1F("enFractionTotal", "fraction of deposited energy;E_{dep} /  E_{MC}; Normalised entries", 1000, 0, 1);
  auto numCells = new TH1F("numCells", "number of cells per event;## cells; Normalised entries", 100, 0, pow(netSize,3) );
  auto enCell = new TH1F("enCell", "cell energy distribution;E_{cell} (MeV); Normalised entries", 1000, 0, floor(1.2 * maxEnergy));
  auto longProfile = new TH1F("longProfile", "longitudinal profilee;t (layer);#LTE#GT (MeV)", netSize, -0.5, netSize - 0.5);
  auto transProfile = new TH1F("transProfile", "transverse profile;r (layer);#LTE#GT (MeV)", netMidCell, - 0.5, netMidCell - 0.5);
  auto enFractionCell = new TH1F("enFractionCell", "cell energy fraction distribution;E_{cell}/E_{MC}; Normalised entries", 1000, 0, 1);
  auto longFirstMoment = new TH1F("longFirstMoment", "longitudinal first moment;#LT#lambda#GT (mm);Normalised entries", netSize*cellSizeMm, 0, netSize * cellSizeMm);
  auto longSecondMoment = new TH1F("longSecondMoment", "longitudinal second moment;#LT#lambda^{2}#GT (mm^{2});Normalised entries", netSize*cellSizeMm, 0, pow(netSize * cellSizeMm, 2));
  auto transFirstMoment = new TH1F("transFirstMoment", "transverse first moment;#LTr#GT (mm);Normalised entries", netSize*cellSizeMm, 0, netSize * cellSizeMm / scaleFactorProfile );
  auto transSecondMoment = new TH1F("transSecondMoment", "transverse second moment;#LTr^{2}#GT (mm^{2});Normalised entries", netSize*cellSizeMm, 0, pow(netSize * cellSizeMm, 2) / pow(scaleFactorProfile, 2));
  auto enLayers = new TH2F("enLayers", "energy distribution per layer;E_{cell} (MeV); layer; Entries", 100, 0, 1000, netSize, -0.5, netSize - 0.5);
  auto enFractionLayers = new TH2F("enFractionLayers", "energy fraction distribution per layer;E_{cell}/E_{MC}; layer; Entries", 1000, 0, 1., netSize, -0.5, netSize - 0.5);
  auto transProfileLayers = new TH2F("transProfileLayers", "transverse profile per layer ;r (layer); layer;#LTE#GT (MeV)", netMidCell, - 0.5, netMidCell - 0.5, netSize, -0.5, netSize - 0.5);

  TTreeReader eventsReader("events",&f);
  TTreeReaderValue<double> energyMC(eventsReader, "EnergyMC");
  TTreeReaderArray<double> energyCellV(eventsReader, "EnergyCell");
  TTreeReaderArray<int> xCellV(eventsReader, "xCell");
  TTreeReaderArray<int> yCellV(eventsReader, "yCell");
  TTreeReaderArray<int> zCellV(eventsReader, "zCell");

  uint iterEvents = 0;
  // retireved from input
  size_t eventSize = 0;
  uint xCell = 0, yCell = 0, zCell = 0;
  double eCell = 0;
  double energyMCinGeV = 0;
  // calculated
  uint tDistance = 0;
  double rDistance = 0, eCellFraction = 0, sumEnergyDeposited = 0;
  double tFirstMoment = 0, tSecondMoment = 0, rFirstMoment = 0, rSecondMoment = 0;
  while(eventsReader.Next()){
    energyMCinGeV = *(energyMC) / 1.e3;
    if (eventSelection) {
      if (energyMCinGeV < minEnergy || energyMCinGeV > maxEnergy) {
        continue;
      }
    }
    sumEnergyDeposited = 0;
    eventSize = energyCellV.GetSize();
    tFirstMoment = 0;
    rFirstMoment = 0;
    for (uint iEntry = 0; iEntry < eventSize; ++iEntry) {
      // get data (missing: angle at entrance)
      xCell = xCellV[iEntry];
      yCell = yCellV[iEntry];
      zCell = zCellV[iEntry];
      eCell = energyCellV[iEntry] * aCellUnitToMeV;
      eCellFraction = eCell / *energyMC;
      // make calculations
      tDistance = zCell; // assumption: particle enters calorimeter perpendiculary
      rDistance = sqrt( (xCell - netMidCell) * (xCell - netMidCell) + (yCell - netMidCell) * (yCell - netMidCell));
      tFirstMoment += eCell * (tDistance + 0.5) * cellSizeMm;
      rFirstMoment += eCell * rDistance * cellSizeMm;
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
    tFirstMoment /= sumEnergyDeposited;
    rFirstMoment /= sumEnergyDeposited;
    tSecondMoment = 0;
    rSecondMoment = 0;
    for (uint iEntry = 0; iEntry < eventSize; ++iEntry) {
      // get data (missing: angle at entrance)
      xCell = xCellV[iEntry];
      yCell = yCellV[iEntry];
      zCell = zCellV[iEntry];
      eCell = energyCellV[iEntry] * aCellUnitToMeV;
      // make calculations
      tDistance = zCell; // assumption: particle enters calorimeter perpendiculary
      rDistance = sqrt( (xCell - netMidCell) * (xCell - netMidCell) + (yCell - netMidCell) * (yCell - netMidCell));
      tSecondMoment += eCell * pow((tDistance + 0.5) * cellSizeMm - tFirstMoment, 2);
      rSecondMoment += eCell * pow(rDistance * cellSizeMm - rFirstMoment, 2);
    }
    tSecondMoment /= sumEnergyDeposited;
    rSecondMoment /= sumEnergyDeposited;
    enMC->Fill( energyMCinGeV );  // convert to GeV
    enTotal->Fill(sumEnergyDeposited / 1.e3);  // convert to GeV
    enFractionTotal->Fill(sumEnergyDeposited / (*energyMC));  // convert to GeV
    numCells->Fill(eventSize);
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

  if(include_simtime) {
    eventsReader.Restart();
    TTreeReaderValue<double> simulationTime(eventsReader, "SimTime");
    while(eventsReader.Next()){
      simTime->Fill(*(simulationTime));
    }
    simTime->Scale(1./iterEvents);
  }
  if(include_simtype) {
    eventsReader.Restart();
    TTreeReaderValue<int> simulationType(eventsReader, "SimType");
    while(eventsReader.Next()){
      simType->Fill(*(simulationType));
    }
    simType->Scale(1./iterEvents);
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
  enLayers->Write();
  enFractionLayers->Write();
  transProfileLayers->Write();
  if(include_simtime) simTime->Write();
  if(include_simtype) simType->Write();
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
  createHistograms(argv[1], outputName);
  return 0;
}
