import sys
import ROOT
from math import sqrt
import argparse
import os

def create_single_canvas(name, title=""):
    if title == "":
        title = name
    c = ROOT.TCanvas(name, title, 1600, 1200)
    c.SetTopMargin(0.01)
    c.SetRightMargin(0.03)
    c.SetLeftMargin(0.15)
    c.SetBottomMargin(0.15)
    ROOT.SetOwnership(c,False)
    return c

def prepare_histogram(histo, title='', colour = 9, markerStyle = 21):
   if title != '':
      histo.SetTitle(title)
   histo.SetMarkerStyle(markerStyle)
   histo.SetMarkerSize(1.4)
   histo.SetMarkerColor(colour)
   histo.SetLineColor(colour)
   # set Y axis
   histo.GetYaxis().SetTitleSize(0.05)
   histo.GetYaxis().SetTitleOffset(1.)
   histo.GetYaxis().SetLabelSize(0.03)
   histo.GetYaxis().SetNdivisions(504)
   # set X axis
   histo.GetXaxis().SetTitleSize(0.05)
   histo.GetXaxis().SetTitleOffset(1.)
   histo.GetXaxis().SetLabelSize(0.03)
   histo.GetYaxis().SetNdivisions(506)

def prepare_graph(graph, name, title, colour = 9, markerStyle = 21, factor = 1):
   # graph settings
   graph.SetTitle(title)
   graph.SetName(name)
   graph.SetMarkerStyle(markerStyle)
   graph.SetMarkerSize(1.6)
   graph.SetMarkerColor(colour)
   graph.SetLineColor(colour)
   # set Y axis
   graph.GetYaxis().SetTitleSize(0.06)
   graph.GetYaxis().SetTitleOffset(0.9)
   graph.GetYaxis().SetLabelSize(0.06)
   graph.GetYaxis().SetNdivisions(504)
   # set X axis
   graph.GetXaxis().SetTitleSize(0.05)
   graph.GetXaxis().SetTitleOffset(1)
   graph.GetXaxis().SetLabelSize(0.06)
   graph.GetXaxis().SetNdivisions(506)

def get_gaus(histo, ifFromMaxBin = False):
    if histo.Integral() < 1e-10:
        return 0,0,0,0
    if ifFromMaxBin:
        binWithMax = 1
        contentBinWithMax = 0
        for iBin in range(1,histo.GetNbinsX()):
            if histo.GetBinContent(iBin) > histo.GetBinContent(binWithMax):
                binWithMax = iBin
        minFitRange = histo.GetBinCenter(binWithMax - 5)
        maxFitRange = histo.GetBinCenter(binWithMax + 5)
    else:
        minFitRange = histo.GetMean() - 2 * histo.GetRMS()
        maxFitRange = histo.GetMean() + 2 * histo.GetRMS()
    # check if fit range is within histogram range
    minFitRange = minFitRange if minFitRange > histo.GetXaxis().GetXmin() else histo.GetXaxis().GetXmin()
    maxFitRange = maxFitRange if maxFitRange < histo.GetXaxis().GetXmax() else histo.GetXaxis().GetXmax()
    myfunPre = ROOT.TF1("firstGaus","gaus", minFitRange, maxFitRange)
    resultPre = histo.Fit(myfunPre, "SQRN")
    minFitRange = resultPre.Get().Parameter(1) - 2. * resultPre.Get().Parameter(2)
    maxFitRange = resultPre.Get().Parameter(1) + 2. * resultPre.Get().Parameter(2)
    minFitRange = minFitRange if minFitRange > histo.GetXaxis().GetXmin() else histo.GetXaxis().GetXmin()
    maxFitRange = maxFitRange if maxFitRange < histo.GetXaxis().GetXmax() else histo.GetXaxis().GetXmax()
    if minFitRange > maxFitRange or maxFitRange < histo.GetXaxis().GetXmin() or minFitRange > histo.GetXaxis().GetXmax():
        minFitRange = histo.GetXaxis().GetXmin()
        maxFitRange = histo.GetXaxis().GetXmax()
    myfun = ROOT.TF1("finalGaus", "gaus", minFitRange, maxFitRange)
    ROOT.SetOwnership(histo, False)
    try:
        result = histo.Fit(myfun, "SRQ+")
        if result:
            return result.Get().Parameter(1), result.Get().Error(1), result.Get().Parameter(2), result.Get().Error(2)
        else:
            return histo.GetMean(), histo.GetRMS()
    except:
        print("ERROR")
        return histo.GetMean(), histo.GetRMS()

def get_gaus_from_projections(histo2d):
    if histo2d.Integral() < 1e-10:
        return [[]]
    num_of_projections = histo2d.GetYaxis().GetNbins()
    values = []
    for layer in range(0,num_of_projections): # bin id starts at 1
        hist = histo2d.ProjectionX("proj"+str(layer),layer, layer+1)
        if hist.Integral() > 1e-10:
            gauss_fit = get_gaus(hist)
            values.append(gauss_fit[0:2])
    print (values)
    return values

def get_gamma(histo, energy):
    ROOT.RooMsgService.instance().setSilentMode(True)
    ROOT.RooMsgService.instance().setGlobalKillBelow(ROOT.RooFit.WARNING);
    if histo.GetIntegral() == 0:
        return 0,0,0,0, 0, 0
    else:
        print("Fit gamma to ", histo.GetName(), "  ", histo.GetEntries())
    gflashZ = 68.3609
    from math import log
    logY = log(energy/0.021)
    gflashT = logY - 0.858
    gflashAlpha = 0.21 + (0.492 + 2.38 / gflashZ ) * logY
    gflashBeta = (gflashAlpha - 1.) / gflashT
    t = ROOT.RooRealVar("t","t",0, 60)
    gammaAlpha = ROOT.RooRealVar("alpha","alpha",gflashAlpha,0,20)
    gammaBeta = ROOT.RooRealVar("beta","beta",1./gflashBeta,0.1,10)
    gammaMu = ROOT.RooRealVar("mu","mu",0)
    Gamma = ROOT.RooGamma("gamma","gamma PDF",t, gammaAlpha, gammaBeta, gammaMu)
    dataSingleLongProfile = ROOT.RooDataHist("dataSingleLongProfile","dataSingleLongProfile",ROOT.RooArgList(t),histo)
    Gamma.fitTo(dataSingleLongProfile, ROOT.RooFit.SumW2Error(True), ROOT.RooFit.PrintEvalErrors(-1), ROOT.RooFit.Verbose(False))
    fitAlpha =  gammaAlpha.getVal()
    fitBeta =  gammaBeta.getVal()
    fitT =  (gammaAlpha.getVal() - 1.) * gammaBeta.getVal()
    fitErrAlpha = gammaAlpha.getError()
    fitErrBeta = gammaBeta.getError()
    fitErrT = (fitAlpha - 1) * fitErrBeta + fitErrAlpha * fitBeta
    return fitAlpha, fitErrAlpha, fitT, fitErrT, fitBeta, fitErrBeta

def get_span(histo, threshold = 0):
    minBin = histo.GetXaxis().GetNbins()
    maxBin = 1
    for ibin in range(1, histo.GetXaxis().GetNbins()):
        if histo.GetBinContent(ibin) > threshold:
            if ibin < minBin:
                minBin = ibin
            if ibin > maxBin:
                maxBin = ibin
    return histo.GetMean(), (histo.GetXaxis().GetBinUpEdge(maxBin) - histo.GetXaxis().GetBinLowEdge(minBin)) / 2.

def calculate(infile, energy):
    calculations = {}
    if energy == 0:
        calculations["enMC"] = get_span(infile.Get("enMC"))
    else:
        calculations["enMC"] = (energy, get_span(infile.Get("enMC"))[1])
    energyResponse = get_gaus(infile.Get("enFractionTotal"))
    enResolution = energyResponse[2] / (energyResponse[0])
    enResolutionErrorSigmaPart = energyResponse[3] / (energyResponse[0])
    enResolutionErrorMeanPart = energyResponse[1] * energyResponse[2] / ( (energyResponse[0]) ** 2)
    enResolutionError = sqrt( enResolutionErrorSigmaPart ** 2 +  enResolutionErrorMeanPart ** 2 )
    calculations["enResolution"] = (enResolution, enResolutionError)
    calculations["enLinearity"] = energyResponse[:2]
    calculations["enDeposited"] = get_gaus(infile.Get("enTotal"))[:2]
    calculations["longFirstMoment"] = get_gaus(infile.Get("longFirstMoment"))[:2]
    calculations["longSecondMoment"] = get_gaus(infile.Get("longSecondMoment"))[:2]
    calculations["transFirstMoment"] = get_gaus(infile.Get("transFirstMoment"))[:2]
    calculations["transSecondMoment"] = get_gaus(infile.Get("transSecondMoment"))[:2]
    calculations["numCells"] = get_gaus(infile.Get("numCells"))[:2]
    calculations["enCell"] = (infile.Get("enCell").GetMean(), infile.Get("enCell").GetRMS())
# avgLongProfileAlpha Beta T -> Fit longProfile and retrieve a b t ? Or rather save in parametrisation.cpp those parameters to the histogram? I'd go for fit in here.
    gamma_long_profile = get_gamma(infile.Get("longProfile"), calculations["enMC"][0])
    calculations["avgLongProfileAlpha"] = gamma_long_profile[:2]
    calculations["avgLongProfileT"] = gamma_long_profile[2:4]
    calculations["avgLongProfileBeta"] = gamma_long_profile[4:]
    singleLongProfileAlpha = get_gaus(infile.Get("longProfileAlpha"))
    calculations["singleLongProfileAlphaMean"] = singleLongProfileAlpha[:2]
    calculations["singleLongProfileAlphaSigma"] = singleLongProfileAlpha[2:]
    singleLongProfileBeta = get_gaus(infile.Get("longProfileBeta"))
    calculations["singleLongProfileBetaMean"] = singleLongProfileBeta[:2]
    calculations["singleLongProfileBetaSigma"] = singleLongProfileBeta[2:]
    singleLongProfileT = get_gaus(infile.Get("longProfileT"))
    calculations["singleLongProfileTMean"] = singleLongProfileT[:2]
    calculations["singleLongProfileTSigma"] = singleLongProfileT[2:]
    singleLongProfileLogAlpha = get_gaus(infile.Get("longProfileLogAlpha"))
    calculations["singleLongProfileLogAlphaMean"] = singleLongProfileLogAlpha[:2]
    calculations["singleLongProfileLogAlphaSigma"] = singleLongProfileLogAlpha[2:]
    singleLongProfileLogBeta = get_gaus(infile.Get("longProfileLogBeta"))
    calculations["singleLongProfileLogBetaMean"] = singleLongProfileLogBeta[:2]
    calculations["singleLongProfileLogBetaSigma"] = singleLongProfileLogBeta[2:]
    singleLongProfileLogT = get_gaus(infile.Get("longProfileLogT"))
    calculations["singleLongProfileLogTMean"] = singleLongProfileLogT[:2]
    calculations["singleLongProfileLogTSigma"] = singleLongProfileLogT[2:]
    calculations["singleLongProfileAlphaTRho"] =[infile.Get("longProfileRhoAlphaT").GetCorrelationFactor(),0]
    calculations["singleLongProfileLogAlphaLogTRho"] = [infile.Get("longProfileRhoLogAlphaLogT").GetCorrelationFactor(),0]
    if infile.Get("simTime"):
        calculations["simTime"] = get_gaus(infile.Get("simTime"), True)
    calculations["numCellsLayers"] = get_gaus_from_projections(infile.Get("numCellsLayers"))
    return calculations

def main(input_names, output_name, energies, energy_critical = 0, X0 = 0, RM = 0, material_name = "unknown"):
    to_plot = ["*"]

    to_calculate_xaxis = {"enMC": ["MC energy"]}
    to_calculate_yaxis = {"enDeposited": ["deposited energy; E_{MC} (GeV);#LTE#GT (GeV)"],
                          "enLinearity": ["energy linearity (fraction); E_{MC} (GeV);#frac{#LTE#GT}{E_{MC}}"],
                          "enResolution": ["energy resolution; E_{MC} (GeV);#frac{#sigma_{E}}{#LTE#GT}"],
                          "longFirstMoment": ["longitudinal first moment; E_{MC} (GeV);#LT #lambda#GT (mm)"],
                          "longSecondMoment": ["longitudinal second moment; E_{MC} (GeV);#LT #lambda^{2}#GT (mm^{2})"],
                          "transFirstMoment": ["transverse first moment; E_{MC} (GeV);#LT r#GT (mm)"],
                          "transSecondMoment": ["transverse second moment; E_{MC} (GeV); #LT r^{2}#GT (mm^{2})"],
                          "numCells": ["number of cells above threshold; E_{MC} (GeV); # cells"],
                          "enCell": ["average cell energy; E_{MC} (GeV); #LTE_{cell}#GT (MeV)"],
                          "avgLongProfileAlpha":  ["average longitudinal profile - #alpha parameter; E_{MC} (GeV);#alpha"],
                          "avgLongProfileBeta": ["average longitudinal profile - #beta parameter; E_{MC} (GeV);#beta"],
                          "avgLongProfileT": ["average longitudinal profile - T_{max} parameter; E_{MC} (GeV);T_{max}"],
                          "singleLongProfileAlphaMean": ["single longitudinal profile - mean #alpha parameter; E_{MC} (GeV);#LT#alpha#GT"],
                          "singleLongProfileBetaMean": ["single longitudinal profile - mean #beta parameter; E_{MC} (GeV);#LT#beta#GT"],
                          "singleLongProfileTMean": ["single longitudinal profile - mean T_{max} parameter; E_{MC} (GeV);#LTT_{max}#GT"],
                          "singleLongProfileAlphaSigma": ["single longitudinal profile - #sigma(#alpha) parameter; E_{MC} (GeV);#sigma(#alpha)"],
                          "singleLongProfileBetaSigma": ["single longitudinal profile - #sigma(#beta) parameter; E_{MC} (GeV);#sigma(#beta)"],
                          "singleLongProfileTSigma": ["single longitudinal profile - #sigma(T_{max}) parameter; E_{MC} (GeV);#sigma(T_{max})"],
                          "singleLongProfileAlphaTRho": ["single longitudinal profile - #rho(#alpha,T_{max}) parameter; E_{MC} (GeV);#rho(#alpha,T_{max})"],
                          "singleLongProfileLogAlphaMean": ["single longitudinal profile - mean log(#alpha) parameter; E_{MC} (GeV);#LTlog(#alpha)#GT"],
                          "singleLongProfileLogBetaMean": ["single longitudinal profile - mean log(#beta) parameter; E_{MC} (GeV);#LTlog(#beta)#GT"],
                          "singleLongProfileLogTMean": ["single longitudinal profile - mean log(T_{max}) parameter; E_{MC} (GeV);#LT log(T_{max})#GT"],
                          "singleLongProfileLogAlphaSigma": ["single longitudinal profile - #sigma(log(#alpha)) parameter; E_{MC} (GeV);#sigma(log(#alpha))"],
                          "singleLongProfileLogBetaSigma": ["single longitudinal profile - #sigma(log(#beta)) parameter; E_{MC} (GeV);#sigma(log(#beta))"],
                          "singleLongProfileLogTSigma": ["single longitudinal profile - #sigma(log(T_{max})) parameter; E_{MC} (GeV);#sigma(log(T_{max}))"],
                          "singleLongProfileLogAlphaLogTRho": ["single longitudinal profile - #rho(log(#alpha),log(T_{max})) parameter; E_{MC} (GeV);#rho(log(#alpha),log(T_{max}))"]}
    to_calculate_yaxis_3d = {"numCellsLayers": ["number of cells above threshold; E_{MC} (GeV); t (layer); # cells"]}

    # calculations from file:
    calc = []
    # histograms from file:
    all_plots = []
    all_files = []

    for iFile, input_file in enumerate(input_names):
        in_root = ROOT.TFile(input_file, "READ")
        all_files.append(in_root)
        if len(energies) > 0:
            calc.append(calculate(in_root, energies[iFile]))
        else:
            calc.append(calculate(in_root, 0))

    if len(to_plot) == 1 and to_plot[0] == "*":
        plot_list_all = []
        for infile in all_files:
            plot_list_per_file = []
            for obj in infile.GetListOfKeys():
                if infile.Get(obj.GetName()).IsA().InheritsFrom("TH1"):
                    plot_list_per_file.append(obj.GetName())
                elif infile.Get(obj.GetName()).IsA().InheritsFrom("TGraph"):
                    plot_list_per_file.append(obj.GetName())
            plot_list_all.append(plot_list_per_file)
        to_plot = list(set.intersection(*map(set,plot_list_all)))
        to_plot_per_layer = [pl for pl in to_plot if pl.find("Layer") != -1]
        to_plot = list(set(to_plot).difference(set(to_plot_per_layer)))

    # extract histograms, fit, fill in graphs
    for iFile, in_root in enumerate(all_files):
        plots_per_file = []
        for iPlot, plot in enumerate(to_plot):
            hist = in_root.Get(plot)
            if not hist:
                sys.exit("Error getting histogram <<"+ plot + ">> from file <<"+ in_root.GetName()+ ">>.")
            hist.SetName("e"+str(int(calc[iFile]["enMC"][0]))+"GeV_"+hist.GetName())
            hist.SetTitle(str(int(calc[iFile]["enMC"][0]))+" GeV, " + hist.GetTitle())
            # TODO instead of always appending, consider merging e.g. for same energy or for same flat energy spectrum
            plots_per_file.append(hist)
        all_plots.append(plots_per_file)

    if "simTime" in to_plot:
        to_calculate_yaxis["simTime"] = ["simulation time (per event); E_{MC} (GeV); #LTt#GT (s)"]

    # check what is the number of layers
    num_layers = all_files[0].Get("numCellsLayers").GetYaxis().GetNbins()

    # new graphs:
    graphs = {}
    graphs3d = {}
    graphsLayers = {}
    graphsEnergy = {}
    for graph_name, graph_params in to_calculate_yaxis.items():
        graph = ROOT.TGraphErrors()
        graph.SetName("g_"+graph_name)
        if energy_critical > 0:
            graph_params[0] = graph_params[0].replace("E_{MC} (GeV)", "y = E_{MC} / E_{critical}")
        graph.SetTitle(graph_params[0])
        for entry in calc:
            if energy_critical > 0:
                to_plot_on_x = entry["enMC"][0] / energy_critical
                to_plot_on_ex = entry["enMC"][1] / energy_critical
            else:
                to_plot_on_x = entry["enMC"][0]
                to_plot_on_ex = entry["enMC"][1]
            graph.SetPoint(graph.GetN(), to_plot_on_x, entry[graph_name][0])
            graph.SetPointError(graph.GetN() - 1, to_plot_on_ex, entry[graph_name][1])
        graphs[graph_name] = graph
    for graph_name, graph_params in to_calculate_yaxis_3d.items():
        graph = ROOT.TGraph2DErrors()
        graph.SetName("g_"+graph_name)
        if energy_critical > 0:
            graph_params[0] = graph_params[0].replace("E_{MC} (GeV)", "y = E_{MC} / E_{critical}")
        graph.SetTitle(graph_params[0])
        for ilayer in range(0,num_layers):
            graph_per_layer = ROOT.TGraphErrors()
            graph_per_layer.SetName("g_"+graph_name+"_layer"+str(ilayer))
            graph_per_layer.SetTitle("number of cells in layer "+str(ilayer))
            if energy_critical > 0:
                graph_per_layer.GetXaxis().SetTitle("E_{MC} (GeV)")
            else:
                graph_per_layer.GetXaxis().SetTitle("y = E_{MC} / E_{critical}")
            graphsLayers[ilayer] = graph_per_layer
        for entry in calc:
            graph_per_energy = ROOT.TGraphErrors()
            graph_per_energy.SetName("g_"+graph_name+"_energy_"+str(int(entry["enMC"][0]))+"GeV")
            graph_per_energy.SetTitle("number of cells for energy "+str(int(entry["enMC"][0]))+" GeV")
            graph_per_energy.GetXaxis().SetTitle("layer ID")
            if energy_critical > 0:
                to_plot_on_x = entry["enMC"][0] / energy_critical
                to_plot_on_ex = entry["enMC"][1] / energy_critical
            else:
                to_plot_on_x = entry["enMC"][0]
                to_plot_on_ex = entry["enMC"][1]
            for ilayer, values_per_layer in enumerate(entry[graph_name]):
                graph.SetPoint(graph.GetN(), to_plot_on_x, ilayer, values_per_layer[0])
                graph.SetPointError(graph.GetN() - 1, to_plot_on_ex, 0.5, values_per_layer[1])
                graphsLayers[ilayer].SetPoint(graphsLayers[ilayer].GetN(), to_plot_on_x, values_per_layer[0])
                graphsLayers[ilayer].SetPointError(graphsLayers[ilayer].GetN() - 1, to_plot_on_ex, values_per_layer[1])
                graph_per_energy.SetPoint(graph_per_energy.GetN(), ilayer, values_per_layer[0])
                graph_per_energy.SetPointError(graph_per_energy.GetN() - 1, 0.5, values_per_layer[1])
            graphsEnergy[graph_per_energy.GetName()] = graph_per_energy
        graphs3d[graph_name] = graph

    # create canvases
    canvases = []
    for iCanv in range(len(to_plot)):
        canvases.append(create_single_canvas("canv_h_"+to_plot[iCanv]))
    for graph_name, graph in graphs.items():
        canvases.append(create_single_canvas("canv_"+graph_name))
    for graph_name, graph in graphs3d.items():
        canvases.append(create_single_canvas("canv_"+graph_name))
    for graph_name, graph in graphsLayers.items():
        canvases.append(create_single_canvas("canv_"+str(graph_name)))

    # draw
    # plots
    for iEnergy, plots_per_energy in enumerate(all_plots):
        for iPlot, hist in enumerate(plots_per_energy):
            canvases[iPlot].cd()
            prepare_histogram(hist)
            if iEnergy == 0:
                hist.Draw()
            else:
                hist.Draw("same")

    # graphs
    for iGraph, graph_name in enumerate(graphs):
        canvases[len(to_plot) + iGraph].cd()
        prepare_graph(graphs[graph_name], "g_"+graph_name, graphs[graph_name].GetTitle())
        graphs[graph_name].Draw("aep")
    for iGraph, graph_name in enumerate(graphs3d):
        canvases[len(to_plot) + len(graphs) + iGraph].cd()
        prepare_graph(graphs3d[graph_name], "g_"+graph_name, graphs3d[graph_name].GetTitle())
        graphs3d[graph_name].Draw("apERR")
    for iGraph, graph_name in enumerate(graphsLayers):
        canvases[len(to_plot) + len(graphs) + +len(graphs3d) + iGraph].cd()
        prepare_graph(graphsLayers[graph_name], "g_"+str(graph_name), graphsLayers[graph_name].GetTitle())
        graphsLayers[graph_name].Draw("aep")
    # store everything in a file
    out_root_perEnergy = ROOT.TFile(output_name[:-5]+"_perEnergy.root", "RECREATE")
    for plots in all_plots:
        for plot in plots:
            plot.Write()
    out_root_perLayer = ROOT.TFile(output_name[:-5]+"_perLayer.root", "RECREATE")
    for graph_name, graph in graphsLayers.items():
        graph.Write()
    for graph_name, graph in graphsEnergy.items():
        graph.Write()
    out_root = ROOT.TFile(output_name, "RECREATE")
    for graph_name, graph in graphs3d.items():
        graph.Write()


    longitudinal_params_to_calculate_lin = ["avgLongProfileAlpha", "avgLongProfileT", "avgLongProfileBeta", "singleLongProfileLogAlphaLogTRho"] #, "singleLongProfileAlphaMean", "singleLongProfileTMean", "singleLongProfileBetaMean"]
    longitudinal_params_to_calculate_log = ["singleLongProfileLogAlphaMean", "singleLongProfileLogTMean", "singleLongProfileLogBetaMean"]
    longitudinal_params_to_calculate_inv = ["singleLongProfileLogAlphaSigma", "singleLongProfileLogTSigma", "singleLongProfileLogBetaSigma"]
    longitudinal_params = {}

    if material_name == "Pb":
        Z = 82.
    elif material_name == "W":
        Z = 74.
    elif material_name == "PbWO4":
        Z  = 68.36
    original_params = {"avgLongProfileT_p0": -0.812, "avgLongProfileT_p1": 1,
                       "avgLongProfileAlpha_p0": 0.21,  "avgLongProfileAlpha_p1": (0.492+2.38/Z),
                       "singleLongProfileLogTMean_p0": -0.812, "singleLongProfileLogTMean_p1": 1,
                       "singleLongProfileLogTSigma_p0": -1.4, "singleLongProfileLogTSigma_p1": 1.26,
                       "singleLongProfileLogAlphaMean_p0": 0.81, "singleLongProfileLogAlphaMean_p1": (0.458+2.26/Z),
                       "singleLongProfileLogAlphaSigma_p0": -0.58, "singleLongProfileLogAlphaSigma_p1": 0.86,
                       "singleLongProfileLogAlphaLogTRho_p0": 0.705, "singleLongProfileLogAlphaLogTRho_p1": -0.023}
    os.system("mkdir -p tex")
    os.system("mkdir -p data")
    for graph_name, graph in graphs.items():
        graph.Write()
        if graph_name.find("Rho")>0:
            ROOT.gROOT.SetBatch(False)
        else:
            ROOT.gROOT.SetBatch(True)
        canv = create_single_canvas("canv")
        if graph_name in longitudinal_params_to_calculate_lin :
            fitfun = ROOT.TF1("fitfun", "[0]+[1]*log(x)", graph.GetXaxis().GetXmin(), graph.GetXaxis().GetXmax(), 2)
        elif graph_name in longitudinal_params_to_calculate_log :
            fitfun = ROOT.TF1("fitfun", "log([0]+[1]*log(x))", graph.GetXaxis().GetXmin(), graph.GetXaxis().GetXmax(), 2)
        elif graph_name in longitudinal_params_to_calculate_inv :
            fitfun = ROOT.TF1("fitfun", "1./([0]+[1]*log(x))", graph.GetXaxis().GetXmin(), graph.GetXaxis().GetXmax(), 2)
        else:
            continue
        # set start parameters to GFlash original parameters
        if graph_name.find('Beta') < 0:
            fitfun.SetParameter(0,original_params[graph_name+"_p0"])
            fitfun.SetParameter(1,original_params[graph_name+"_p1"])
        fitres = graph.Fit(fitfun, "RSM")
        print(fitres.Get().Chi2(), fitres.Get().Parameter(0), fitres.Get().Error(0), fitres.Get().Parameter(1), fitres.Get().Error(1))
        longitudinal_params[graph_name+"_p0"] = fitres.Get().Parameter(0)
        longitudinal_params[graph_name+"_e0"] = fitres.Get().Error(0)
        longitudinal_params[graph_name+"_p1"] = fitres.Get().Parameter(1)
        longitudinal_params[graph_name+"_e1"] = fitres.Get().Error(1)
        pad = canv.cd()
        pad.SetLogx()
        tmp = graph.GetTitle()
        graph.SetTitle("")
        graph.Draw()
        if graph_name.find('Beta') < 0:
            fitfun.SetParameter(0,original_params[graph_name+"_p0"])
            fitfun.SetParameter(1,original_params[graph_name+"_p1"])
            fitfun.SetLineColor(3)
            fitfun.Draw("same")
            if graph_name.find("Rho")>0:
                graph.GetYaxis().SetRangeUser(0.4, graph.GetMaximum())
        canv.Print("tex/"+graph_name+".pdf")
        canv.Print("tex/"+graph_name+".root")
    print (longitudinal_params)
    out_data = open("data/" + output_name[:-5] + "G4_" + material_name + ".dat","w+")
    p = longitudinal_params
    save_for_geant = [p["avgLongProfileT_p0"], p["avgLongProfileT_p1"], p["avgLongProfileAlpha_p0"], p["avgLongProfileAlpha_p1"],
                  p["singleLongProfileLogTSigma_p0"], p["singleLongProfileLogTSigma_p1"],
                  p["singleLongProfileLogAlphaSigma_p0"], p["singleLongProfileLogAlphaSigma_p1"],
                  p["singleLongProfileLogAlphaLogTRho_p0"], p["singleLongProfileLogAlphaLogTRho_p1"]]
    out_data.write(str(len(save_for_geant))+"\n")
    for param in save_for_geant:
        out_data.write(str(param) + "\n")
    out_data.close()
    out_tex = open("tex/" + output_name[:-4] + "tex","w+")
    out_tex.write(r'''\documentclass[12pt]{article}''')
    out_tex.write(r'''\usepackage{graphicx}\usepackage[margin=2cm]{geometry}''')
    out_tex.write(r'''\begin{document}\section{Detector}\begin{itemize}''')
    out_tex.write("\item Material: " + material_name)
    out_tex.write("\item $ X_0 = " + str(X0) + "$")
    out_tex.write("\item $ R_M = " + str(RM) + "$")
    out_tex.write("\item $ E_C = " + str(EC) + "$")
    out_tex.write("\end{itemize}\section{Parametrisation}")
    out_tex.write(r'''\begin{equation}dE(\overline{r})=Ef(t)dt f(r) dr f(\varphi) \varphi\end{equation}''')
    out_tex.write(r'''\centering$ y = \frac{E_{MC}}{E_C}$''')
    out_tex.write("\subsection{Longitudinal profile}")
    out_tex.write(r'''\begin{equation} f(t) = \frac{\left(\left(\alpha-1\right) t\right)^{\alpha-1} \left(\alpha-1\right) \exp^{-\beta t}}{T \cdot\Gamma(\alpha)}\label{eq:longProfileAlphaT}\end{equation}''')
#    out_tex.write(r'''\begin{equation} f(t) = \frac{\left(\beta t\right)^{\alpha-1} \beta \exp^{-\beta t}}{\Gamma(\alpha)}~~~~~~ \left(T = \frac{\alpha - 1}{\beta}\right)\end{equation}''')


    out_tex.write("\subsubsection{Parameters}")
    symbol = {"T": "T", "Alpha": r'''\alpha''', "LogT": "\log T", "LogAlpha": r'''\log\alpha'''}
    digits = 3
    for param in ["T", "Alpha"]:
        out_tex.write(r'''\begin{equation} ''' + symbol[param] + ' = ' + str(round(longitudinal_params["avgLongProfile"+param+"_p1"],digits)) + ' \cdot \log y ' + ('+' if longitudinal_params["avgLongProfile"+param+"_p0"] > 0 else '') + str(round(longitudinal_params["avgLongProfile"+param+"_p0"],digits)) + '\label{eq:longProfileParamsAvg'+param+'}\end{equation}')
    for param in ["T", "Alpha"]:
        out_tex.write(r'''\begin{equation} \langle\log ''' + symbol[param] + r'''\rangle = \log\left(''' + str(round(longitudinal_params["singleLongProfileLog"+param+"Mean_p1"],digits)) + ' \cdot \log y ' + ('+' if longitudinal_params["singleLongProfileLog"+param+"Mean_p0"] > 0 else '') + str(round(longitudinal_params["singleLongProfileLog"+param+"Mean_p0"],digits)) + r'''\right) \label{eq:longProfileParamsSingleLog'''+param+r'''Mean}\end{equation}''')
        out_tex.write(r'''\begin{equation} \sigma\left(\log ''' + symbol[param] + r'''\right) = \left(''' + str(round(longitudinal_params["singleLongProfileLog"+param+"Sigma_p1"],digits)) + ' \cdot \log y ' + ('+' if longitudinal_params["singleLongProfileLog"+param+"Sigma_p0"] > 0 else '') + str(round(longitudinal_params["singleLongProfileLog"+param+"Sigma_p0"],digits)) + r'''\right)^{-1} \label{eq:longProfileParamsSingleLog'''+param+r'''Sigma}\end{equation}''')
    out_tex.write(r'''\begin{equation}  \rho\left(\log T, \log\alpha \right)= ''' + str(round(longitudinal_params["singleLongProfileLogAlphaLogTRho_p1"],digits)) + ' \cdot \log y ' + ('+' if longitudinal_params["singleLongProfileLogAlphaLogTRho_p0"] > 0 else '') + str(round(longitudinal_params["singleLongProfileLogAlphaLogTRho_p0"],digits)) + '\label{eq:longProfileParamsSingleRho}\end{equation}')
    out_tex.write("\clearpage")
    out_tex.write("\subsubsection{Average profiles}\\  ")
    for param in ["T", "Alpha"]:
        out_tex.write(r'''\begin{figure}[!ht]\centering\includegraphics[height=0.3\textheight]{tex/'''+"avgLongProfile"+param+".pdf}\caption{"+r'''$''' + symbol[param] + r'''$ parameter extracted from the fit to the average longitudinal shower profile with Gamma distribution, Eq.~(\ref{eq:longProfileAlphaT}).'''+" Fitted function (red) is described by $" + symbol[param] + ' = (' + str(round(longitudinal_params["avgLongProfile"+param+"_p1"],digits)) + ' \pm ' + str(abs(round(longitudinal_params["avgLongProfile"+param+"_e1"],digits))) + ') \cdot \log y + (' + str(round(longitudinal_params["avgLongProfile"+param+"_p0"],digits)) + ' \pm '+ str(round(longitudinal_params["avgLongProfile"+param+"_e0"],digits)) + r''')$, Eq.~(\ref{eq:longProfileParamsAvg'''+param+"}). Original GFlash parameters are plotted in green for$ Z_{\mathrm{effective}}$ = "+str(int(Z))+": $"+symbol[param]+" = "+ str(round(original_params["avgLongProfile"+param+"_p1"],digits)) + ' \cdot \log y ' + ('+' if original_params["avgLongProfile"+param+"_p0"] > 0 else '') + str(round(original_params["avgLongProfile"+param+"_p0"],digits)) +"$~\cite{org}.}\end{figure}")
    out_tex.write("\clearpage")
    out_tex.write("\subsubsection{Individual profiles}\\  ")
    for param in ["T", "Alpha"]:
        out_tex.write(r'''\begin{figure}[!ht]\centering\includegraphics[height=0.3\textheight]{tex/'''+"singleLongProfileLog"+param+"Mean.pdf}\caption{"+r'''Mean of the Gaussian distribution of $\log '''+symbol[param]+r'''$ parameter, where $'''+symbol[param]+r'''$ is extracted from the fit to the individual shower profile with Gamma distribution, Eq.~(\ref{eq:longProfileAlphaT}).'''+" Fitted function (red) is described by $\langle\log " + symbol[param] + r''' \rangle = \log\left( (''' + str(round(longitudinal_params["singleLongProfileLog"+param+"Mean_p1"],digits)) + ' \pm ' + str(abs(round(longitudinal_params["singleLongProfileLog"+param+"Mean_e1"],digits))) + ') \cdot \log y + (' + str(round(longitudinal_params["singleLongProfileLog"+param+"Mean_p0"],digits)) + ' \pm '+ str(round(longitudinal_params["singleLongProfileLog"+param+"Mean_e0"],digits)) + r''')\right)$, Eq.~(\ref{eq:longProfileParamsSingleLog'''+param+r'''Mean}). Original GFlash parameters are plotted in green for$ Z_{\mathrm{effective}}$ = '''+str(int(Z))+": $\langle\log "+symbol[param]+r'''\rangle = \log \left( '''+ str(round(original_params["singleLongProfileLog"+param+"Mean_p1"],digits)) + ' \cdot \log y ' + ('+' if original_params["singleLongProfileLog"+param+"Mean_p0"] > 0 else '') + str(round(original_params["singleLongProfileLog"+param+"Mean_p0"],digits)) +r'''\right)$~\cite{org}.}\end{figure}''')
        out_tex.write(r'''\begin{figure}[!ht]\centering\includegraphics[height=0.3\textheight]{tex/'''+"singleLongProfileLog"+param+"Sigma.pdf}\caption{"+r'''Sigma of the Gaussian distribution of $\log '''+symbol[param]+r'''$ parameter, where $'''+symbol[param]+r'''$ is extracted from the fit to the individual shower profile with Gamma distribution, Eq.~(\ref{eq:longProfileAlphaT}).'''+" Fitted function (red) is described by $\sigma\left(\log " + symbol[param] + r''' \right) = \left( (''' + str(round(longitudinal_params["singleLongProfileLog"+param+"Sigma_p1"],digits)) + ' \pm ' + str(abs(round(longitudinal_params["singleLongProfileLog"+param+"Sigma_e1"],digits))) + ') \cdot \log y + (' + str(round(longitudinal_params["singleLongProfileLog"+param+"Sigma_p0"],digits)) + ' \pm '+ str(round(longitudinal_params["singleLongProfileLog"+param+"Sigma_e0"],digits)) + r''')\right)^{-1}$, Eq.~(\ref{eq:longProfileParamsSingleLog'''+param+r'''Sigma}). Original GFlash parameters are plotted in green for$ Z_{\mathrm{effective}}$ = '''+str(int(Z))+": $\sigma\left(\log "+symbol[param]+r'''\right) = \left( '''+ str(round(original_params["singleLongProfileLog"+param+"Sigma_p1"],digits)) + ' \cdot \log y ' + ('+' if original_params["singleLongProfileLog"+param+"Sigma_p0"] > 0 else '') + str(round(original_params["singleLongProfileLog"+param+"Sigma_p0"],digits)) +r'''\right)^{-1}$~\cite{org}.}\end{figure}''')
    out_tex.write(r'''\begin{figure}\centering\includegraphics[height=0.3\textheight]{tex/'''+"singleLongProfileLogAlphaLogTRho.pdf}\caption{"+r'''Correlation between $\log T$ and $\log\alpha$ parameters, where $T$ and $\alpha$ are extracted from the fit to the individual shower profile with Gamma distribution, Eq.~(\ref{eq:longProfileAlphaT}).'''+r''' Fitted function (red) is described by $ \rho(\log T,\log\alpha)  = (''' + str(round(longitudinal_params["singleLongProfileLogAlphaLogTRho_p1"],digits)) + ' \pm ' + str(abs(round(longitudinal_params["singleLongProfileLogAlphaLogTRho_e1"],digits))) + ') \cdot \log y + (' + str(round(longitudinal_params["singleLongProfileLogAlphaLogTRho_p0"],digits)) + ' \pm '+ str(round(longitudinal_params["singleLongProfileLogAlphaLogTRho_e0"],digits)) + r''')$, Eq.~(\ref{eq:longProfileParamsSingleRho}). Original GFlash parameters are plotted in green for$ Z_{\mathrm{effective}}$ = '''+str(int(Z))+r''': $\rho(\log T, \log\alpha) = '''+ str(round(original_params["singleLongProfileLogAlphaLogTRho_p1"],digits)) + ' \cdot \log y ' + ('+' if original_params["singleLongProfileLogAlphaLogTRho_p0"] > 0 else '') + str(round(original_params["singleLongProfileLogAlphaLogTRho_p0"],digits)) +"$~\cite{org}.}\end{figure}")
    out_tex.write("\clearpage\subsection{Transverse profile}")
    out_tex.write(r'''\begin{equation} f(\varphi) = \frac{1}{2\pi}\end{equation}''')
    out_tex.write(r'''\begin{thebibliography}{50}\bibitem{org}  G.~Grindhammer and S.~Peters,\emph{``The Parameterized simulation of electromagnetic showers in homogeneous and sampling calorimeters,''} hep-ex/0001020.\end{thebibliography}''')
    out_tex.write("\end{document}")
    out_tex.close()
    os.system("cat " + output_name[:-4] + "tex")
    os.system("pdflatex -aux_directory=tex/ -output-directory=tex/ tex/" + output_name[:-4] + "tex")
    os.system("pdflatex -aux_directory=tex/ -output-directory=tex/ tex/" + output_name[:-4] + "tex")
    ## TODO print params to text/root file

    ## TODO print params to tex file for comparison (end execute pdflatex?)
    # os.system("pdflatex " + output_name[:-5] + ".tex")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Combine shower validation plots')
    parser.add_argument('inputs', type=str, nargs='+', help="name of the input files")
    parser.add_argument('--output', "-o", dest='output', type=str, default="combinedValidation.root", help="name of the output file")
    parser.add_argument('--energies', "-e", dest='energies', type=float, nargs='+', default=[], help="assign energies to files")
    parser.add_argument('--visual', "-v", dest='visual', action='store_true', help="If plots should be also displayed.")
    parser.add_argument('--ecritical', "-ec", dest='ecritical', type=float,default=0, help="Critical energy. If defined, y=E/Ec is used instead of E.")
    parser.add_argument('--material', "-m", required=True, choices=['Pb', 'W', 'PbWO4'], type=str, help="Material to define X0, RM and Ec")
    parser.add_argument('--document', "-d", action="store_true", help="If true documentation in LaTeX is generated")
    args = parser.parse_args()
    if args.material == "Pb":
        X0 = 5.612
        RM = 16.02
        EC = 0.00743
    elif args.material == "W":
        X0 = 3.504
        RM = 9.327
        EC = 0.00797
    elif args.material == "PbWO4":
        X0 = 8.903
        RM = 16.02
        EC = 0.00964
    else:
        exit("Unknown material")
    if args.ecritical: # allow overwrite
        EC = args.ecritical
    if not args.visual:
       ROOT.gROOT.SetBatch(True)
    if len(args.energies) > 0 and len(args.energies) != len(args.inputs):
        print("If energies are defined, there must be one value per file.")
    main(args.inputs, args.output, args.energies, EC, X0, RM, args.material)
    if args.visual:
       raw_input("Press ENTER to exit")
