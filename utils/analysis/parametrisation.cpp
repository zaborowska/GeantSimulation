#include <iostream>
#include <iomanip>
#include "TH1.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TFitResult.h"
#include "ROOT/RDataFrame.hxx"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooGamma.h"
#include "RooPlot.h"
//#include "createHistograms.h"

void parametrisation(const std::string& aInput, const std::string& aOutput, double X0, double RM, double EC, uint numCellsR = 200, uint numCellsZ = 60, double cellSizeRinMM = 0.98, double cellSizeZinMM = 4.45, uint rebinTransverse = 1) {
  RooMsgService::instance().setSilentMode(true);
  RooMsgService::instance().setGlobalKillBelow(RooFit::WARNING);

  TFile f(aInput.c_str(), "READ");

  // Set initial parameters
  const uint netSizeR = numCellsR; // 200 bins of width 0.98mm = 0.1RM (RM = 19.6mm)
  const uint rebinR = rebinTransverse;
  const uint netSizeZ = numCellsZ; // 60 bins of width 4.45mm = 0.5X0 (X0=8.9mm)
  const double cellSizeMmR = cellSizeRinMM;
  const double cellSizeMmZ = cellSizeZinMM;
  //
  // recalculate lengths in units of Xo, RM, EC
  const double maxLengthR = netSizeR * cellSizeMmR / RM;
  const double maxLengthZ = netSizeZ * cellSizeMmZ / X0;
  const double rId2r = cellSizeMmR / RM;
  const double zId2t = cellSizeMmZ / X0;

  std::cout << "\n\n\n max lengths = R z : " << maxLengthR << "\t" << maxLengthZ << "\t" <<
    " conversion factors : " << rId2r << "\t" << zId2t << std::endl << std::endl << std::endl;
  //
  double minEnergy = 0;
  double maxEnergy = 0;
  const double scaleFactorProfile = 10.; // to shorten the axis in some cases (otherwise most entries in the very beginning only)
  const double netMidCellR = floor (netSizeR / 2);

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
    std::cout << std::endl << "Flat energy spectrum simulation" << std::endl;
    return;
  }
  //
  // recalculate lengths in units of Xo, RM, EC
  const double logY = log( maxEnergy / EC );
  // GFlash paper values hep-ex/0001020v1
  const double gflashZ = 68.3609; // from G4 (GVFlashShowerParameterisation::Material)
  const double gflashT = logY - 0.858;
  const double gflashAlpha = 0.21 + (0.492 + 2.38 / gflashZ ) * logY;
  const double gflashBeta = (gflashAlpha - 1.) / gflashT;
  RooRealVar t("t","t",0, maxLengthZ) ;
  RooRealVar gammaAlpha("alpha","alpha",gflashAlpha,0,20) ;
  RooRealVar gammaBeta("beta","beta",1./gflashBeta,0.1,10) ; // due to different representation in RooFit
  RooRealVar gammaMu("mu","mu",0) ; // different representation in RooFit, no mu in our fit
  RooGamma Gamma("gamma","gamma PDF",t, gammaAlpha, gammaBeta, gammaMu) ;
  //
  // check if SimTime exists in ROOT file
  bool include_simtime = false, include_simtype = false, include_generated_params = false;
  auto col_names = d.GetColumnNames();
  if (std::find(col_names.begin(), col_names.end(), "SimTime") != col_names.end()) {
      include_simtime = true;
  }
  if (std::find(col_names.begin(), col_names.end(), "SimType") != col_names.end()) {
      include_simtype = true;
  }
  if (std::find(col_names.begin(), col_names.end(), "GflashParams") != col_names.end()) {
      include_generated_params = true;
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
  auto mesh = new TH3F("mesh", "mesh", netSizeZ, 0, maxLengthZ, netSizeR, - maxLengthR, maxLengthR, netSizeR, -maxLengthR, maxLengthR);
  mesh->SetTitle(";z (X_{0});x (R_{M});y (R_{M})");
  auto enMC = new TH1F("enMC", "MC energy (GeV);E_{MC} (GeV); Normalised entries", 1000, 0, floor(1.2 * maxEnergy));
  auto enTotal = new TH1F("enTotal", "total deposited energy (GeV);E_{dep} (GeV); Normalised entries", 1000, 0, floor(1.2 * maxEnergy));
  auto enFractionTotal = new TH1F("enFractionTotal", "fraction of deposited energy;E_{dep} /  E_{MC}; Normalised entries", 1000, 0, 0);
  auto numCells = new TH1F("numCells", "number of cells above 0.5 keV per event;## cells; Normalised entries", 100, 0, 0 );
  auto enCell = new TH1F("enCell", "cell energy distribution;E_{cell} (MeV); Normalised entries", 1000, 0, 0);
  auto logEnCell = new TH1F("logEnCell", "cell energy distribution;log(E_{cell} (MeV)); Normalised entries", 1000, 0, 0);
  auto longProfile = new TH1F("longProfile", "longitudinal profile;t (X_{0});#LTE#GT (MeV)", netSizeZ, 0, maxLengthZ);
  auto longProfileSingle = new TH1F("longProfileSingle", "longitudinal profile for a single shower;t (X_0);#LTE#GT (MeV)", netSizeZ, 0, maxLengthZ);
  auto longProfileAlpha = new TH1F("longProfileAlpha", "longitudinal profile fit - alpha parameter;#alpha;Entries", 100, 1, 10);
  auto longProfileBeta = new TH1F("longProfileBeta", "longitudinal profile fit - beta parameter;#beta;Entries", 100, 0, 2);
  auto longProfileT = new TH1F("longProfileT", "longitudinal profile fit - shower maximum;T_{max};Entries", 100, 1, 20);
  auto longProfileLogAlpha = new TH1F("longProfileLogAlpha", "longitudinal profile fit - log alpha parameter;log(#alpha);Entries", 100, 0, 3);
  auto longProfileLogBeta = new TH1F("longProfileLogBeta", "longitudinal profile fit - log beta parameter;log(#beta);Entries", 100, -3, 1);
  auto longProfileLogT = new TH1F("longProfileLogT", "longitudinal profile fit - log shower maximum;log(T_{max});Entries", 100, 1, 3);
//  auto longProfileRhoAlphaT = new TH2F("longProfileRhoAlphaT", "longitudinal profile fit correlation - alpha and T;#alpha;T_{max};Entries", 100, 1, 10, 100, 1, 20);
//  auto longProfileRhoLogAlphaLogT = new TH2F("longProfileRhoLogAlphaLogT", "longitudinal profile fit correlation - log alpha and log T;log(#alpha);log(T_{max});Entries", 100, 0, 3, 100, 1, 3);
  auto longProfileRhoAlphaT = new TGraph();
  longProfileRhoAlphaT->SetName("longProfileRhoAlphaT");
  longProfileRhoAlphaT->SetTitle("longitudinal profile fit correlation - alpha and T;#alpha;T_{max};Entries");
  auto longProfileRhoLogAlphaLogT = new TGraph();
  longProfileRhoLogAlphaLogT->SetName("longProfileRhoLogAlphaLogT");
  longProfileRhoLogAlphaLogT->SetTitle("longitudinal profile fit correlation - log alpha and log T;log(#alpha);log(T_{max});Entries");
  auto transProfile = new TH1F("transProfile", "transverse profile;r (R_{M});#LTE#GT (MeV)", netMidCellR / rebinR, 0, maxLengthR);
  auto enFractionCell = new TH1F("enFractionCell", "cell energy fraction distribution;E_{cell}/E_{MC}; Normalised entries", 100, 0, 0);
//  enFractionCell->SetBit(TH1::kCanRebin);
  auto longFirstMoment = new TH1F("longFirstMoment", "longitudinal first moment;#LT#lambda#GT (mm);Normalised entries", 100, 0, 0); //netSizeZ*cellSizeMmZ, 0, netSizeZ * cellSizeMmZ);
  auto longSecondMoment = new TH1F("longSecondMoment", "longitudinal second moment;#LT#lambda^{2}#GT (mm^{2});Normalised entries", 100, 0, 0); // netSizeZ*cellSizeMmZ, 0, pow(netSizeZ * cellSizeMmZ, 2));
  auto transFirstMoment = new TH1F("transFirstMoment", "transverse first moment;#LTr#GT (mm);Normalised entries",  100, 0, 0); //netSizeR*cellSizeMmR, 0, netSizeR * cellSizeMmR / scaleFactorProfile );
  auto transSecondMoment = new TH1F("transSecondMoment", "transverse second moment;#LTr^{2}#GT (mm^{2});Normalised entries",  100, 0, 0); //netSizeR*cellSizeMmR, 0, pow(netSizeR * cellSizeMmR, 2) / pow(scaleFactorProfile, 2));
  auto numCellsLayers = new TH2F("numCellsLayers", "number of cells distribution per layer;## cells; t (X_{0}); Entries", 1000, 0, 0, netSizeZ, 0, maxLengthZ);
  auto enLayers = new TH2F("enLayers", "energy distribution per layer;E_{cell} (MeV); t (X_{0}); Entries", 1000, 0, 0, netSizeZ, 0, maxLengthZ);
  auto logEnLayers = new TH2F("logEnLayers", "energy distribution per layer;log(E_{cell} (MeV)); t (X_{0}); Entries", 1000, 0, 0, netSizeZ, 0, maxLengthZ);
  auto enFractionLayers = new TH2F("enFractionLayers", "energy fraction distribution per layer;E_{cell}/E_{MC}; t (X_{0}); Entries", 1000, 0, 0, netSizeZ, 0, maxLengthZ);
  auto transProfileLayers = new TH2F("transProfileLayers", "transverse profile per layer ;r (R_{M}); layer;#LTE#GT (MeV)", (netMidCellR*2+1) / rebinR, -maxLengthR, maxLengthR, netSizeZ, 0, maxLengthZ);
  auto transXProfileLayers = new TH2F("transXProfileLayers", "transverse X profile per layer ;x (R_{M}); layer;#LTE#GT (MeV)", netSizeR, 0, 2*maxLengthR, netSizeZ, 0, maxLengthZ);
  auto transYProfileLayers = new TH2F("transYProfileLayers", "transverse Y profile per layer ;y (R_{M}); layer;#LTE#GT (MeV)", netSizeR, 0, 2*maxLengthR, netSizeZ, 0, maxLengthZ);
  // validation plots (from G4 GFlash)
  auto g4generatedLongProfileLogAlphaMean = new TH1F("g4generatedLongProfileLogAlphaMean", "longitudinal profile - G4 generated - log alpha parameter;#LTlog(#alpha)#GT;Entries", 100, 1, 3);
  auto g4generatedLongProfileLogAlphaSigma = new TH1F("g4generatedLongProfileLogAlphaSigma", "longitudinal profile - G4 generated - log alpha parameter;#sigma(log(#alpha));Entries", 100, 0, 3);
  auto g4generatedLongProfileLogTMean = new TH1F("g4generatedLongProfileLogTMean", "longitudinal profile - G4 generated - log shower maximum;#LTlog(T_{max})#GT;Entries", 100, 1, 3);
  auto g4generatedLongProfileLogTSigma = new TH1F("g4generatedLongProfileLogTSigma", "longitudinal profile - G4 generated - log shower maximum;#sigma(log(T_{max}));Entries", 100, 0, 3);
  auto g4generatedLongProfileAlpha = new TH1F("g4generatedLongProfileAlpha", "longitudinal profile - G4 generated - alpha parameter;#alpha;Entries", 100, 1, 10);
  auto g4generatedLongProfileBeta = new TH1F("g4generatedLongProfileBeta", "longitudinal profile - G4 generated - beta parameter;#beta;Entries", 100, 0, 2);
  auto g4generatedLongProfileT = new TH1F("g4generatedLongProfileT", "longitudinal profile - G4 generated - shower maximum;T_{max};Entries", 100, 1, 20);
  auto g4generatedLongProfileRhoLogAlphaLogT = new TH1F("g4generatedLongProfileRhoLogAlphaLogT", "longitudinal profile - G4 generated - correlation;#rho(log(#alpha),log(T_{max}));Entries", 100, -1, 1);
  auto g4generatedLongProfileRandom1 = new TH1F("g4generatedLongProfileRandom1", "longitudinal profile - G4 generated - random 1;random_{1};Entries", 100, -5, 5);
  auto g4generatedLongProfileRandom2 = new TH1F("g4generatedLongProfileRandom2", "longitudinal profile - G4 generated - random 2;random_{2};Entries", 100, -5, 5);

  TTreeReader eventsReader("events",&f);
  TTreeReaderValue<double> energyMC(eventsReader, "EnergyMC");
  TTreeReaderArray<double> energyCellV(eventsReader, "EnergyCell");
  TTreeReaderArray<int> rhoCellV(eventsReader, "rhoCell");
  TTreeReaderArray<int> phiCellV(eventsReader, "phiCell");
  TTreeReaderArray<int> zCellV(eventsReader, "zCell");

  uint iterEvents = 0;
  // retireved from input
  size_t eventSize = 0;
  std::vector<uint> numCellsPerLayer;
  numCellsPerLayer.reserve(netSizeZ);
  uint rhoCell = 0, phiCell = 0, zCell = 0;
  double eCell = 0;
  double energyMCinGeV = 0;
  // calculated
  double tDistance = 0;
  double rDistance = 0, eCellFraction = 0, sumEnergyDeposited = 0;
  double tFirstMoment = 0, tSecondMoment = 0, rFirstMoment = 0, rSecondMoment = 0;
  double xDistance = 0, yDistance = 0;
  while(eventsReader.Next()){
    energyMCinGeV = *(energyMC) / 1.e3;
    sumEnergyDeposited = 0;
    eventSize = energyCellV.GetSize();
    tFirstMoment = 0;
    rFirstMoment = 0;
    longProfileSingle->Reset();
    for (uint iLayer = 0; iLayer < netSizeZ; ++iLayer) {
      numCellsPerLayer[iLayer] = 0;
    }
    for (uint iEntry = 0; iEntry < eventSize; ++iEntry) {
      // get data (missing: angle at entrance)
      rhoCell = rhoCellV[iEntry];
      phiCell = phiCellV[iEntry];
      zCell = zCellV[iEntry];
      eCell = energyCellV[iEntry];
      eCellFraction = eCell / *energyMC;
      // make calculations
      tDistance = zCell + 0.5; // assumption: particle enters calorimeter perpendiculary
      int signX = 1;
      if (rhoCell < netMidCellR) {
        signX = -1;
      }
      rDistance = signX * sqrt( (rhoCell - netMidCellR) * (rhoCell - netMidCellR) + (phiCell - netMidCellR) * (phiCell - netMidCellR));
      tFirstMoment += eCell * tDistance * cellSizeMmZ;
      rFirstMoment += eCell * rDistance * cellSizeMmR;
      // fill histograms
      mesh->Fill(tDistance*zId2t, (rhoCell - netMidCellR) * rId2r, (phiCell-netMidCellR) * rId2r, eCell);
      enCell->Fill(eCell);
      logEnCell->Fill(log(eCell));
      enLayers->Fill(eCell, tDistance*zId2t);
      logEnLayers->Fill(log(eCell), tDistance*zId2t);
      longProfile->Fill(tDistance*zId2t, eCell);
      longProfileSingle->Fill(tDistance*zId2t, eCell);
      transProfile->Fill(rDistance*rId2r, eCell);
      transProfileLayers->Fill(rDistance*rId2r, tDistance*zId2t, eCell);
      transXProfileLayers->Fill(rhoCell*rId2r, tDistance*zId2t, eCell);
      transYProfileLayers->Fill(phiCell*rId2r, tDistance*zId2t, eCell);
      enFractionCell->Fill(eCellFraction);
      enFractionLayers->Fill(eCellFraction, tDistance*zId2t);
      sumEnergyDeposited += eCell;
      numCellsPerLayer[zCell] ++;
    }
    tFirstMoment /= sumEnergyDeposited;
    rFirstMoment /= sumEnergyDeposited;
    tSecondMoment = 0;
    rSecondMoment = 0;
    for (uint iEntry = 0; iEntry < eventSize; ++iEntry) {
      // get data (missing: angle at entrance)
      rhoCell = rhoCellV[iEntry];
      phiCell = phiCellV[iEntry];
      zCell = zCellV[iEntry];
      eCell = energyCellV[iEntry];
      // make calculations
      tDistance = zCell+0.5; // assumption: particle enters calorimeter perpendiculary
      rDistance = sqrt( (rhoCell - netMidCellR) * (rhoCell - netMidCellR) + (phiCell - netMidCellR) * (phiCell - netMidCellR));
      tSecondMoment += eCell * pow(tDistance * cellSizeMmZ - tFirstMoment, 2);
      rSecondMoment += eCell * pow(rDistance * cellSizeMmR - rFirstMoment, 2);
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
    for (uint iLayer = 0; iLayer < netSizeZ; ++iLayer) {
      numCellsLayers->Fill(numCellsPerLayer[iLayer], (iLayer + 0.5)*zId2t);
    }
    iterEvents++;
    // event-by-event calculations:
    RooDataHist dataSingleLongProfile("dataSingleLongProfile","dataSingleLongProfile",t,RooFit::Import(*longProfileSingle)) ;
    Gamma.fitTo(dataSingleLongProfile, RooFit::SumW2Error(kTRUE), RooFit::PrintEvalErrors(-1), RooFit::Verbose(false)) ;
    longProfileT->Fill( (gammaAlpha.getVal() - 1.) * gammaBeta.getVal());
    longProfileAlpha->Fill( gammaAlpha.getVal());
    longProfileBeta->Fill( 1. / gammaBeta.getVal());
    longProfileLogT->Fill(log( (gammaAlpha.getVal() - 1.) * gammaBeta.getVal()));
    longProfileLogAlpha->Fill(log( gammaAlpha.getVal()));
    longProfileLogBeta->Fill(log( 1. / gammaBeta.getVal()));
    // longProfileRhoAlphaT->Fill( gammaAlpha.getVal(), (gammaAlpha.getVal() - 1.) * gammaBeta.getVal() );
    // longProfileRhoLogAlphaLogT->Fill( log(gammaAlpha.getVal()), log((gammaAlpha.getVal() - 1.) * gammaBeta.getVal()) );
    longProfileRhoAlphaT->SetPoint(longProfileRhoAlphaT->GetN(),  gammaAlpha.getVal(), (gammaAlpha.getVal() - 1.) * gammaBeta.getVal());
    longProfileRhoLogAlphaLogT->SetPoint(longProfileRhoLogAlphaLogT->GetN(),  log(gammaAlpha.getVal()), log((gammaAlpha.getVal() - 1.) * gammaBeta.getVal()));
  }
  // Normalize
  enMC->Scale(1./iterEvents);
  enTotal->Scale(1./iterEvents);
  enFractionTotal->Scale(1./iterEvents);
  numCells->Scale(1./iterEvents);
  enCell->Scale(1./iterEvents);
  logEnCell->Scale(1./iterEvents);
  longProfile->Scale(1./iterEvents);
  longProfileAlpha->Scale(1./iterEvents);
  longProfileBeta->Scale(1./iterEvents);
  longProfileT->Scale(1./iterEvents);
  transProfile->Scale(1./iterEvents);
  mesh->Scale(1./iterEvents);
  enFractionCell->Scale(1./iterEvents);
  longFirstMoment->Scale(1./iterEvents);
  longSecondMoment->Scale(1./iterEvents);
  transFirstMoment->Scale(1./iterEvents);
  transSecondMoment->Scale(1./iterEvents);
  numCellsLayers->Scale(1./iterEvents);
  enLayers->Scale(1./iterEvents);
  logEnLayers->Scale(1./iterEvents);
  enFractionLayers->Scale(1./iterEvents);
  transProfileLayers->Scale(1./iterEvents);
transXProfileLayers->Scale(1./iterEvents);
transYProfileLayers->Scale(1./iterEvents);

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
  if ( include_generated_params ) {
    eventsReader.Restart();
    TTreeReaderArray<double> generatedParams(eventsReader, "GflashParams");
    while(eventsReader.Next()){
      g4generatedLongProfileLogTMean->Fill(generatedParams[0]);
      g4generatedLongProfileLogTSigma->Fill(generatedParams[1]);
      g4generatedLongProfileRhoLogAlphaLogT->Fill(generatedParams[2]);
      g4generatedLongProfileT->Fill(generatedParams[3]);
      g4generatedLongProfileLogAlphaMean->Fill(generatedParams[4]);
      g4generatedLongProfileLogAlphaSigma->Fill(generatedParams[5]);
      g4generatedLongProfileAlpha->Fill(generatedParams[6]);
      g4generatedLongProfileBeta->Fill(generatedParams[7]);
      g4generatedLongProfileRandom1->Fill(generatedParams[8]);
      g4generatedLongProfileRandom2->Fill(generatedParams[9]);
    }
    g4generatedLongProfileLogAlphaMean->Scale(1./iterEvents);
    g4generatedLongProfileLogAlphaSigma->Scale(1./iterEvents);
    g4generatedLongProfileLogTMean->Scale(1./iterEvents);
    g4generatedLongProfileLogTSigma->Scale(1./iterEvents);
    g4generatedLongProfileAlpha->Scale(1./iterEvents);
    g4generatedLongProfileBeta->Scale(1./iterEvents);
    g4generatedLongProfileT->Scale(1./iterEvents);
    g4generatedLongProfileRhoLogAlphaLogT->Scale(1./iterEvents);
    g4generatedLongProfileRandom1->Scale(1./iterEvents);
    g4generatedLongProfileRandom2->Scale(1./iterEvents);
  }

  // Define longitudinal profile fit function
  RooDataHist dataAvgLongProfile("dataAvgLongProfile","dataAvgLongProfile",t,RooFit::Import(*longProfile)) ;
  RooPlot* frame = t.frame(RooFit::Title("Gamma p.d.f.")) ;
  dataAvgLongProfile.plotOn(frame) ;
  // fit average profile
  Gamma.plotOn(frame, RooFit::LineColor(3));
  Gamma.fitTo(dataAvgLongProfile, RooFit::SumW2Error(kTRUE), RooFit::PrintEvalErrors(-1), RooFit::Verbose(false)) ;
  Gamma.plotOn(frame);
  TCanvas canvLongProfile("canvLongProfile", "Average longitudinal profile");
  canvLongProfile.cd();
  frame->Draw();
  std::cout << std::endl << std::setfill('=') << std::setw(40) << '\n';
  std::cout << "Parameters form arXiv::hep-ex/0001020:" << std::endl;
  std::cout << "T_hom = " << gflashT << std::endl;
  std::cout << "alpha_hom = " << gflashAlpha << std::endl;
  std::cout << "beta_hom = " << gflashBeta << std::endl;
  std::cout << std::setfill('=') << std::setw(40) <<  '\n' << std::endl;
  std::cout << std::endl << std::setfill('=') << std::setw(40) << '\n';
  std::cout << "Parameters form fit:" << std::endl;
  std::cout << "T_hom = " << ( gammaAlpha.getVal() - 1.) * gammaBeta.getVal() << " +- " <<
    gammaAlpha.getError() * gammaBeta.getVal() + gammaBeta.getError() *  ( gammaAlpha.getVal() - 1.) << std::endl;
  std::cout << "alpha_hom = " << gammaAlpha.getVal() << " +- " << gammaAlpha.getError() << std::endl;
  std::cout << "beta_hom = " << 1. / gammaBeta.getVal() << " +- " << gammaBeta.getError() / pow(gammaBeta.getVal(),2) << std::endl;
  std::cout << std::setfill('=') << std::setw(40) <<  '\n' << std::endl;

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
  logEnCell->Write();
  longProfile->Write();
  longProfileAlpha->Write();
  longProfileBeta->Write();
  longProfileT->Write();
  longProfileLogAlpha->Write();
  longProfileLogBeta->Write();
  longProfileLogT->Write();
  longProfileRhoAlphaT->Write();
  longProfileRhoLogAlphaLogT->Write();
  transProfile->Write();
  enFractionCell->Write();
  longFirstMoment->Write();
  longSecondMoment->Write();
  transFirstMoment->Write();
  transSecondMoment->Write();
  enLayers->Write();
  numCellsLayers->Write();
  logEnLayers->Write();
  enFractionLayers->Write();
  transProfileLayers->Write();
  transXProfileLayers->Write();
  transYProfileLayers->Write();
  if(include_simtime) simTime->Write();
  if(include_simtype) simType->Write();
  if(include_generated_params) {
    g4generatedLongProfileLogAlphaMean->Write();
    g4generatedLongProfileLogAlphaSigma->Write();
    g4generatedLongProfileLogTMean->Write();
    g4generatedLongProfileLogTSigma->Write();
    g4generatedLongProfileAlpha->Write();
    g4generatedLongProfileBeta->Write();
    g4generatedLongProfileT->Write();
    g4generatedLongProfileRhoLogAlphaLogT->Write();
    g4generatedLongProfileRandom1->Write();
    g4generatedLongProfileRandom2->Write();
  }
  canvLongProfile.Write();
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
  // for PbWO4
  const double X0 = 8.903; // mm
  const double RM = 19.59; // mm
  const double EC = 9.64; // MeV
  parametrisation(argv[1], outputName, X0, RM, EC);
  return 0;
}
