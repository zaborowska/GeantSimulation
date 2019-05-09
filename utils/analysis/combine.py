import sys
import ROOT
from math import sqrt
import argparse

def create_single_canvas(name, title=""):
    if title == "":
        title = name
    c = ROOT.TCanvas(name, title, 800, 600)
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
   graph.GetYaxis().SetTitleOffset(1.05)
   graph.GetYaxis().SetLabelSize(0.06)
   graph.GetYaxis().SetNdivisions(504)
   # set X axis
   graph.GetXaxis().SetTitleSize(0.06)
   graph.GetXaxis().SetTitleOffset(1)
   graph.GetXaxis().SetLabelSize(0.06)
   graph.GetXaxis().SetNdivisions(506)

def get_gaus(histo, with_errors = False):
    myfunPre = ROOT.TF1("firstGaus","gaus")
    resultPre = histo.Fit(myfunPre, "SQN")
    myfun = ROOT.TF1("finalGaus", "gaus", resultPre.Get().Parameter(1) - 2. * resultPre.Get().Parameter(2),
                     resultPre.Get().Parameter(1) + 2. * resultPre.Get().Parameter(2) )
    result = histo.Fit(myfun, "SRQN")
    if with_errors:
        return result.Get().Parameter(1), result.Get().Parameter(2), result.Get().Error(1), result.Get().Error(2)
    return result.Get().Parameter(1), result.Get().Parameter(2)

def get_span(histo, threshold = 0):
    minBin = histo.GetXaxis().GetNbins()
    maxBin = 0
    for ibin in range(0, histo.GetXaxis().GetNbins()):
        if histo.GetBinContent(ibin) > threshold:
            if ibin < minBin:
                minBin = ibin
            if ibin > maxBin:
                maxBin = ibin
    return histo.GetMean(), (histo.GetXaxis().GetBinUpEdge(maxBin) - histo.GetXaxis().GetBinLowEdge(minBin)) / 2.

def calculate(infile):
    calculations = {}
    calculations["enMC"] = get_span(infile.Get("enMC"))
    energyResponse = get_gaus(infile.Get("enTotal"), True)
    enResolution = energyResponse[1] / (energyResponse[0])
    enResolutionErrorSigmaPart = energyResponse[3] / (energyResponse[0])
    enResolutionErrorMeanPart = energyResponse[2] * energyResponse[1] / ( (energyResponse[0]) ** 2)
    enResolutionError = sqrt( enResolutionErrorSigmaPart ** 2 +  enResolutionErrorMeanPart ** 2 )
    calculations["enDeposited"] = energyResponse[:2]
    calculations["enResolution"] = (enResolution, enResolutionError)
    energyResponse = get_gaus(infile.Get("enFractionTotal"), True)
    calculations["enLinearity"] = get_gaus(infile.Get("enFractionTotal"))
    calculations["longFirstMoment"] = get_gaus(infile.Get("longFirstMoment"))
    calculations["longSecondMoment"] = get_gaus(infile.Get("longSecondMoment"))
    calculations["transFirstMoment"] = get_gaus(infile.Get("transFirstMoment"))
    calculations["transSecondMoment"] = get_gaus(infile.Get("transSecondMoment"))
    calculations["numCells"] = get_gaus(infile.Get("numCells"))
    calculations["enCell"] = (infile.Get("enCell").GetMean(), infile.Get("enCell").GetRMS())
    return calculations

def main(input_names, output_name):

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
                          "enCell": ["average cell energy; E_{MC} (GeV); #LTE_{cell}#GT (MeV)"]}

    # calculations from file:
    calc = []
    # histograms from file:
    all_plots = []
    all_files = []

    for input_file in input_names:
        in_root = ROOT.TFile(input_file, "READ")
        all_files.append(in_root)
        calc.append(calculate(in_root))

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

    # new graphs:
    graphs = {}
    for graph_name, graph_params in to_calculate_yaxis.items():
        graph = ROOT.TGraphErrors()
        graph.SetName("g_"+graph_name)
        graph.SetTitle(graph_params[0])
        for entry in calc:
            graph.SetPoint(graph.GetN(), entry["enMC"][0], entry[graph_name][0])
            graph.SetPointError(graph.GetN() - 1, entry["enMC"][1], entry[graph_name][1])
        graphs[graph_name] = graph

    # create canvases
    canvases = []
    for iCanv in range(len(to_plot)):
        canvases.append(create_single_canvas("canv_h_"+to_plot[iCanv]))
    for graph_name, graph in graphs.items():
        print (graph_name, graph)
        canvases.append(create_single_canvas("canv_"+graph_name))

    # draw
    # plots
    for iEnergy, plots_per_energy in enumerate(all_plots):
        for iPlot, hist in enumerate(plots_per_energy):
            canvases[iPlot].cd()
            prepare_histogram(hist)
            if iEnergy == 0:
                hist.Draw()
                print(hist.GetName())
            else:
                hist.Draw("same")

    # graphs
    for iGraph, graph_name in enumerate(graphs):
        canvases[len(to_plot) + iGraph].cd()
        prepare_graph(graphs[graph_name], "g_"+graph_name, graphs[graph_name].GetTitle())
        graphs[graph_name].Draw("aep")

    # store everything in a file
    out_root = ROOT.TFile(output_name, "RECREATE")
    for plots in all_plots:
        for plot in plots:
            plot.Write()
    for graph_name, graph in graphs.items():
        graph.Write()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Combine shower validation plots')
    parser.add_argument('inputs', type=str, nargs='+', help="name of the input files")
    parser.add_argument('--output', "-o", dest='output', type=str, default="combinedValidation.root", help="name of the output file")
    parser.add_argument('--visual', "-v", dest='visual', action='store_true', help="If plots should be also displayed.")
    args = parser.parse_args()
    if not args.visual:
       ROOT.gROOT.SetBatch(True)
    main(args.inputs, args.output)
    if args.visual:
       raw_input("Press ENTER to exit")
