import sys
import ROOT
from math import sqrt
import argparse
import warnings

def prepare_canvas(name, title):
   c = ROOT.TCanvas(name, title, 1500, 900)
   ROOT.SetOwnership(c,False)
   return c

def prepare_legend(legend):
   legend.SetFillColor(0)
   legend.SetMargin(0.15)
   legend.SetFillStyle(0)
   legend.SetLineColor(0)
   legend.SetLineWidth(0)
   legend.SetShadowColor(10)
   legend.SetTextSize(0.03)
   legend.SetTextFont(42)
   legend.Clear()

def graph2hist(graph, bin_params):
   x_errors = graph.GetEX()
   if len(bin_params) == 0:
      first_run = True
      bin_params["min"] = graph.GetXaxis().GetXmin()
      bin_params["max"] = graph.GetXaxis().GetXmax()
      bin_params["width"] = max(x_errors)
      bin_params["N"] = int((bin_params["max"] - bin_params["min"]) / bin_params["width"])
      bin_params["x_values"] = graph.GetX()
   else:
      first_run = False
   hist = ROOT.TH1D("h_" + graph.GetName().replace("g_",""),graph.GetTitle()+";"+graph.GetXaxis().GetTitle()+";"+graph.GetYaxis().GetTitle(),
                    bin_params["N"], bin_params["min"], bin_params["max"])
   for iPoint in range(graph.GetN()):
      current_x_value = graph.GetX()[iPoint]
      if not first_run:
         for x in bin_params["x_values"]:
            if current_x_value - x_errors[iPoint] < x and x < current_x_value + x_errors[iPoint]:
               current_x_value = x
               break
      bin_id = hist.FindBin(current_x_value)
      hist.SetBinContent(bin_id, graph.GetY()[iPoint])
      hist.SetBinError(bin_id, graph.GetEY()[iPoint])
      print(graph.GetName(), "  setting " , bin_id, "\t", graph.GetY()[iPoint], "\t", graph.GetEY()[iPoint] )
   ROOT.SetOwnership(hist,False)
   return hist, bin_params

def setYaxisMinMax(listOfPlots):
   if len(listOfPlots) == 0:
      return
   minim = -1
   maxim = -1
   for plot in listOfPlots:
      if plot.IsA().InheritsFrom("TH1"):
         plot_max = plot.GetMaximum()
         # avoid minimum to be == 0
         plot_min = plot.GetMaximum()
         for iBin in range(0,plot.GetNbinsX()):
            content = plot.GetBinContent(iBin)
            if content != 0 and content < plot_min :
               plot_min = content
      elif plot.IsA().InheritsFrom("TGraph"):
         plot_max = plot.GetYaxis().GetXmax()
         # avoid minimum to be == 0
         plot_min = plot.GetYaxis().GetXmax()
         y_values = plot.GetY()
         for yVal in y_values:
            if yVal != 0 and yVal < plot_min:
               plot_min = yVal
      if minim == -1 or minim > plot_min:
         minim = plot_min
      if maxim == -1 or maxim < plot_max:
         maxim = plot_max
   if plot.IsA().InheritsFrom("TH1"):
      listOfPlots[0].SetMinimum(0.9*minim)
      listOfPlots[0].SetMaximum(1.1*maxim)
   elif plot.IsA().InheritsFrom("TGraph"):
      listOfPlots[0].GetYaxis().SetRangeUser(minim, maxim)

def setXaxisMinMax(listOfPlots):
   if len(listOfPlots) == 0:
      return
   for plot in listOfPlots:
      if plot.IsA().InheritsFrom("TH1"):
         minBin = 0
         maxBin = plot.GetNbinsX()
         for iBin in range(minBin,maxBin):
            if plot.GetBinContent(iBin) > 0:
               minim = plot.GetBinLowEdge(iBin)
               break
         for iBin in range(0, maxBin):
            if plot.GetBinContent(maxBin - 1 - iBin) > 0:
               maxim = plot.GetBinLowEdge(maxBin - 1 - iBin) + plot.GetBinWidth(maxBin - 1 - iBin)
               break
      elif plot.IsA().InheritsFrom("TGraph"):
         x_values = plot.GetX()
         minim = min(x_values)
         maxim = max(x_values)
   for plot in listOfPlots:
      plot.GetXaxis().SetRangeUser(minim, maxim)

def copyStyle(obj1, obj2):
   obj1.SetMarkerStyle(obj2.GetMarkerStyle())
   obj1.SetMarkerSize(obj2.GetMarkerSize())
   obj1.SetMarkerColor(obj2.GetMarkerColor())
   obj1.SetLineColor(obj2.GetLineColor())
   obj1.SetLineWidth(obj2.GetLineWidth())

def main(input_names, output_name, legend_names = [], histograms = []):
   in_root = []
   for input_name in input_names:
      in_root.append(ROOT.TFile(input_name, "READ"))
   # store everything in a file
   out_root_hist = ROOT.TFile(output_name.replace(".root","_perMCEnergy.root"), "RECREATE")
   out_root_graph = ROOT.TFile(output_name.replace(".root","_fncOfMCEnergy.root"), "RECREATE")
   if len(input_names) > 1:
      # retrieving same-named histogram from different files yields warning
      warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='Replacing existing TH1' )
   # division of histograms translated from graph wll result in fake 0-division warnings
   warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='Number of graph points is different than histogram bins' )

   if not histograms:
      plot_list_all = []
      for iFile, infile in enumerate(in_root):
         plot_list_per_file = []
         for obj in infile.GetListOfKeys():
            if infile.Get(obj.GetName()).IsA().InheritsFrom("TH1") and not infile.Get(obj.GetName()).IsA().InheritsFrom("TH2") and not infile.Get(obj.GetName()).IsA().InheritsFrom("TH3"):
               plot_list_per_file.append(obj.GetName())
            elif infile.Get(obj.GetName()).IsA().InheritsFrom("TGraph"):
               plot_list_per_file.append(obj.GetName())
         plot_list_all.append(plot_list_per_file)
      histograms = list(set.intersection(*map(set,plot_list_all)))

    # STYLE options
   ROOT.gStyle.SetOptStat(0)
   colours = [ROOT.kBlue+1,   ROOT.kRed-4,  ROOT.kBlack,
              ROOT.kGreen+2, ROOT.kOrange-3, ROOT.kRed-4, ROOT.kBlue+1, ROOT.kMagenta+2, ROOT.kCyan-3, 9, ROOT.kTeal-1]
   symbols = [ 21, 20, 34, 24, 25, 28, 22, 26]

   for hist_name in histograms:
      canvas = prepare_canvas("canv_"+hist_name.replace("h_","").replace("g_",""), hist_name.replace("h_","").replace("g_",""))
      hists = []
      ratios = []
      bin_params = {}
      for iFile, infile in enumerate(in_root):
         hist = infile.Get(hist_name)
         if not hist:
            sys.exit("Error getting histogram <<"+ hist_name+ ">> from file <<"+ infile.GetName()+ ">>.")
         if hist.IsA().InheritsFrom("TGraph"):
            hist, bin_params = graph2hist(hist, bin_params)
         hist.SetLineColor(colours[iFile])
         hist.SetFillColor(colours[iFile])
         hist.SetMarkerColor(colours[iFile])
         hist.SetMarkerStyle(symbols[iFile%len(symbols)])
         hist.SetMarkerSize(1.6)
         hists.append(hist)
         ROOT.SetOwnership(hist, False)
         ratio = ROOT.TRatioPlot(hist, hists[0])
         ratio_graph = ratio.GetCalculationOutputGraph()
         copyStyle(ratio_graph, hist)
         ROOT.SetOwnership(ratio, False)
         ROOT.SetOwnership(ratio_graph, False)
         ratios.append(ratio_graph)
         if iFile == 0:
            ratio.SetGraphDrawOpt("c")
            ratio.SetH1DrawOpt("ep")
            ratio.SetH2DrawOpt("ep")
            ratio.Draw()
            baseline = ratio
            setXaxisMinMax([ratios[0], ratio_graph])
         else:
            baseline.GetLowerPad().cd()
            ratio_graph.Draw("sameep")
            setYaxisMinMax([ratios[0], ratio_graph])
            baseline.GetLowerPad().Update()
            baseline.GetUpperPad().cd()
            hist.Draw("sameep")
            setYaxisMinMax([hists[0], hist])
            canvas.Update()

      legend = baseline.GetUpperPad().BuildLegend()
      prepare_legend(legend)
      for hist, entryname in zip(hists, legend_names):
         legend.AddEntry(hist, entryname, "ep")
      for ih, h in enumerate(hists):
         h.SetName(h.GetName()+"_"+input_names[ih].replace(".root",""))
         if  h.GetName().find("GeV_") != -1:
            out_root_hist.cd()
         else:
            out_root_graph.cd()
         h.Write()
      for ig, g in enumerate(ratios):
         g.SetName("ratio_"+hists[ig].GetName().replace("h_","g_"))
         if g.GetName().find("GeV_") != -1:
            out_root_hist.cd()
         else:
            out_root_graph.cd()
         g.Write()
      canvas.Update()
      if canvas.GetName().find("GeV_") != -1:
         out_root_hist.cd()
      else:
         out_root_graph.cd()
      canvas.Write()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Combine shower validation plots')
    parser.add_argument('inputs', type=str, nargs='+', help="Name of the input files.")
    parser.add_argument('--output', "-o", dest='output', type=str, default="combinedValidation.root", help="Name of the output file.")
    parser.add_argument("--histogramName","-n", help="Name of the plots to compare (default: all).", type = str, nargs='+')
    parser.add_argument('--visual', "-v", dest='visual', action='store_true', help="If plots should be also displayed.")
    parser.add_argument('--legend', "-l", dest='legend', type=str, nargs="+", default = [], help="Labels to be displayed in legend.")
    args = parser.parse_args()
    if len(args.legend) == 0:
       legend_names = args.inputs
    elif len(args.legend) != len(args.inputs):
       sys.exit("Number of labels for legend ("+str(len(args.legend))+") must be equal to the number of input files ("+str(len(args.inputs))+").")
    else:
       legend_names = args.legend
    if not args.visual:
       ROOT.gROOT.SetBatch(True)
    main(args.inputs, args.output, legend_names, args.histogramName)
    if args.visual:
       raw_input("Press ENTER to exit")
