import sys
import ROOT
import os
from math import sqrt
import argparse
import warnings

def prepare_canvas(name, title):
   c = ROOT.TCanvas(name, title, 2000, 900)
   ROOT.SetOwnership(c,False)
   return c

def prepare_graph(graph, name, title, colour = 9, fillcolour = 9, markerStyle = 21, fillstyle = 3001, factor = 1):
   # graph settings
   graph.SetTitle(title)
   graph.SetName(name)
   graph.SetMarkerStyle(markerStyle)
   graph.SetMarkerSize(1.5)
   graph.SetMarkerColor(colour)
   graph.SetLineColor(colour)
   graph.SetFillColor(fillcolour)
   graph.SetFillStyle(fillstyle)
   graph.SetLineWidth(2)
   # set Y axis
   graph.GetYaxis().SetTitleSize(0.05)
   graph.GetYaxis().SetTitleOffset(0.9)
   graph.GetYaxis().SetLabelSize(0.045)
   graph.GetYaxis().SetNdivisions(504)
   graph.GetYaxis().SetMaxDigits(4)
   # set X axis
   graph.GetXaxis().SetTitleSize(0.05)
   graph.GetXaxis().SetTitleOffset(1.1)
   graph.GetXaxis().SetLabelSize(0.05)
   graph.GetYaxis().SetNdivisions(506)

def prepare_second_graph(secondary, main, name, title, factor = 2):
   # graph settings
   secondary.SetTitle("")
   secondary.SetName(name)
   secondary.SetMarkerStyle(main.GetMarkerStyle())
   secondary.SetMarkerSize(main.GetMarkerSize())
   secondary.SetMarkerColor(main.GetMarkerColor())
   secondary.SetLineColor(main.GetLineColor())
   # set X axis
   main.GetXaxis().SetTitleOffset(1.7)
   secondary.GetXaxis().SetTitle(main.GetXaxis().GetTitle())
   secondary.GetXaxis().SetLabelSize(main.GetXaxis().GetLabelSize()*factor)
   secondary.GetXaxis().SetLabelOffset(main.GetXaxis().GetLabelOffset()/factor)
   secondary.GetXaxis().SetTitleSize(main.GetXaxis().GetTitleSize()*factor)
   secondary.GetXaxis().SetTitleOffset(main.GetXaxis().GetTitleOffset()/factor)
   secondary.GetXaxis().SetTickLength(main.GetXaxis().GetTickLength())
   secondary.GetXaxis().SetNdivisions(506)
   main.GetXaxis().SetLabelSize(0)
   main.GetXaxis().SetTitleSize(0)
   main.GetXaxis().SetTickLength(main.GetXaxis().GetTickLength()*factor)
   # set Y axis
   main.GetYaxis().CenterTitle()
   secondary.GetYaxis().CenterTitle()
   secondary.GetYaxis().SetTitle("ratio")
   secondary.GetYaxis().SetLabelSize(main.GetYaxis().GetLabelSize()*factor)
   secondary.GetYaxis().SetLabelOffset(main.GetYaxis().GetLabelOffset()/factor)
   secondary.GetYaxis().SetTitleSize(main.GetYaxis().GetTitleSize()*factor)
   secondary.GetYaxis().SetTitleOffset(main.GetYaxis().GetTitleOffset()/factor)
   secondary.GetYaxis().SetTickLength(main.GetYaxis().GetTickLength())
   secondary.GetYaxis().SetNdivisions(506)

def prepare_single_canvas(name, title):
   c = ROOT.TCanvas(name, title, 2000, 900)
   c.SetTopMargin(0.1)
   c.SetRightMargin(0.03)
   c.SetLeftMargin(0.1)
   c.SetBottomMargin(0.15)
   ROOT.SetOwnership(c,False)
   return c

def prepare_double_canvas(name, title, factor = 1):
   c = ROOT.TCanvas(name, title, 2000, int(900 + 900 * factor))
   pad1 = ROOT.TPad("pad1","pad1",0, 0, 1, factor / (1. + factor))
   pad2 = ROOT.TPad("pad2","pad2",0,factor / (1. + factor),1,1)
   pad2.SetBottomMargin(0.02)
   pad2.SetRightMargin(0.03)
   pad2.SetLeftMargin(0.1)
   pad1.SetBorderMode(0)
   pad1.SetTopMargin(0.1)
   pad1.SetRightMargin(0.03)
   pad1.SetLeftMargin(0.1)
   pad1.SetBottomMargin(0.2)
   pad2.SetBorderMode(0)
   pad1.SetGridy()
   pad1.SetTickx(1)
   pad2.SetTickx(1)
   pad1.SetTicky(1)
   pad2.SetTicky(1)
   # pad1.SetLogx()
   # pad2.SetLogx()
   pad1.Draw()
   pad2.Draw()
   ROOT.SetOwnership(c,False)
   ROOT.SetOwnership(pad1,False)
   ROOT.SetOwnership(pad2,False)
   pad1.cd()
   return c, pad1, pad2

def prepare_legend(legend):
   legend.SetFillColor(0)
   legend.SetMargin(0.15)
   legend.SetFillStyle(0)
   legend.SetLineColor(0)
   legend.SetLineWidth(0)
   legend.SetShadowColor(10)
   legend.SetTextSize(0.04)
   legend.SetTextFont(42)
   legend.Clear()

def graphErrors2hist(graph, bin_params):
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
   ROOT.SetOwnership(hist,False)
   return hist, bin_params

def graphDivide(graphNum, graphDen):
   x_vals = graphDen.GetX()
   ratio = ROOT.TGraph()
   for ix in x_vals:
      ratio.SetPoint(ratio.GetN(), ix, graphNum.Eval(ix)/ graphDen.Eval(ix))
   ROOT.SetOwnership(ratio,False)
   return ratio

def graph2hist(graph, bin_params):
   if len(bin_params) == 0:
      first_run = True
      bin_params["min"] = graph.GetXaxis().GetXmin()
      bin_params["max"] = graph.GetXaxis().GetXmax()
      bin_params["N"] = 100
      bin_params["width"] = int((bin_params["max"] - bin_params["min"]) / bin_params["N"])
      bin_params["x_values"] = graph.GetX()
   else:
      first_run = False
   hist = ROOT.TH1D("h_" + graph.GetName().replace("g_",""),graph.GetTitle()+";"+graph.GetXaxis().GetTitle()+";"+graph.GetYaxis().GetTitle(),
                    bin_params["N"], bin_params["min"], bin_params["max"])
   for iPoint in range(graph.GetN()):
      current_x_value = graph.GetX()[iPoint]
      if not first_run:
         for x in bin_params["x_values"]:
            if current_x_value - 0.5 * bin_params["width"] < x and x < current_x_value + 0.5 * bin_params["width"]:
               current_x_value = x
               break
      bin_id = hist.FindBin(current_x_value)
      hist.SetBinContent(bin_id, graph.GetY()[iPoint])
      print(graph.GetName(), "  setting " , bin_id, "\t", graph.GetY()[iPoint])
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
         plot_max = max(plot.GetY())
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
   if minim < 1e-3:
      minim = 0
   if plot.IsA().InheritsFrom("TH1"):
      listOfPlots[0].SetMinimum(0.9*minim)
      listOfPlots[0].SetMaximum(1.1*maxim)
   elif plot.IsA().InheritsFrom("TGraph"):
      listOfPlots[0].GetYaxis().SetRangeUser(0.9*minim, 1.1*maxim)

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
         print("X = ", x_values)
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

def main(input_names, output_name, legend_names = [], legend_short_names = [], histograms = [], noratio = False):
   in_root = []
   for input_name in input_names:
      in_root.append(ROOT.TFile(input_name, "READ"))
   if len(input_names) > 1:
      # retrieving same-named histogram from different files yields warning
      warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='Replacing existing TH1' )
   # division of histograms translated from graph wll result in fake 0-division warnings
   warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='Number of graph points is different than histogram bins' )
   warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='Sum of squares of weights structure already created' )

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

   print("Histograms that files have in common: ", histograms)
   # STYLE options
   ROOT.gStyle.SetOptStat(000)
   colours = [ROOT.kBlue+1,   ROOT.kRed-4,  ROOT.kBlack,
              ROOT.kGreen+2, ROOT.kOrange-3, ROOT.kRed-4, ROOT.kBlue+1, ROOT.kMagenta+2, ROOT.kCyan-3, 9, ROOT.kTeal-1]
   colours_fill = [ROOT.kAzure-4,   ROOT.kRed-9,  ROOT.kGray+2,
              ROOT.kGreen+2, ROOT.kOrange-3, ROOT.kRed-4, ROOT.kBlue+1, ROOT.kMagenta+2, ROOT.kCyan-3, 9, ROOT.kTeal-1]
   symbols = [ 21, 20, 34, 24, 25, 28, 22, 26]
   fillstyle = [ 1001, 3004, 3005, 3011, 3014, 3020, 3025, 3018]

   for hist_name in histograms:
      if noratio:
         canvas = prepare_single_canvas("canv_"+hist_name.replace("h_","").replace("g_",""), hist_name.replace("h_","").replace("g_",""))
         up = canvas.cd()
      else:
         canvas, down, up = prepare_double_canvas("canv_"+hist_name.replace("h_","").replace("g_",""), hist_name.replace("h_","").replace("g_",""), 0.5)
      legend = ROOT.TLegend(0.6,0.87 - 0.06 * len(in_root),0.9,0.85)
      prepare_legend(legend)
      hists = []
      ratios = []
      bin_params = {}
      print(hist_name)
      for iFile, infile in enumerate(in_root):
         hist = infile.Get(hist_name)
         if not hist:
            sys.exit("Error getting histogram <<"+ hist_name+ ">> from file <<"+ infile.GetName()+ ">>.")
         # hist.SetLineColor(colours[iFile])
         # hist.SetFillColor(colours[iFile])
         # hist.SetMarkerColor(colours[iFile])
         # hist.SetMarkerStyle(symbols[iFile%len(symbols)])
         # hist.SetMarkerSize(1.6)
         hists.append(hist)
         ROOT.SetOwnership(hist, False)
         prepare_graph(hist, hist.GetName(), hist.GetTitle(), colours[iFile%len(colours)], colours_fill[iFile%len(colours_fill)], symbols[iFile%len(symbols)], fillstyle[iFile%len(fillstyle)])
         up.cd()
         if iFile == 0:
            if hist.IsA().InheritsFrom("TGraph"):
               if "fit" in infile.GetName():
                  hist.Draw("ac")
               else:
                  hist.Draw("aep")
            else:
               hist.Draw("epbar")
         else:
            if "fit" in infile.GetName():
               hist.Draw("samec")
            else:
               if hist.IsA().InheritsFrom("TGraph"):
                  hist.Draw("sameep")
               else:
                  hist.Draw("sameepbar")
            setYaxisMinMax([hists[0], hist])
         if not noratio:
            if hist.IsA().InheritsFrom("TGraph"):
               ratio = graphDivide(hist, hists[0])
         #            copyStyle(ratio, hist)
            else:
               ratio = hist.Clone("ratio"+hist.GetName())
               ratio.Sumw2()
               ROOT.SetOwnership(ratio, False)
               ratio.Divide(hists[0])
            ratios.append(ratio)
            prepare_second_graph(ratio, hist, ratio.GetName(), ratio.GetTitle(), 2)
            down.cd()
            if iFile == 0:
               if ratio.IsA().InheritsFrom("TGraph"):
                  if "fit" in infile.GetName():
                     ratio.Draw("ac")
                  else:
                     ratio.Draw("aep")
               else:
                  if "fit" in infile.GetName():
                     ratio.Draw("c")
                  else:
                     ratio.Draw("ep")
            else:
               if "fit" in infile.GetName():
                  ratio.Draw("samec")
               else:
                  ratio.Draw("sameep")
               setYaxisMinMax([ratios[0], ratio])
         canvas.Update()
      for hist, entryname in zip(hists, legend_names):
         if "fit" in entryname:
            legend.AddEntry(hist, entryname, "l")
         else:
            legend.AddEntry(hist, entryname, "ep")
      up.cd()
      legend.Draw()
      canvas.Update()
      os.system("mkdir -p " + output_name)
      if noratio:
         hist_name += "_noratio"
      canvas.SaveAs(output_name+"/"+hist_name+".pdf")
      outfile = ROOT.TFile(output_name+"/"+hist_name+".root","RECREATE")
      outfile.cd()
      canvas.Write()
      for ih, h in enumerate(hists):
         h.SetName(h.GetName()+"_"+legend_short_names[ih])
         h.Write()
      outfile.Close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Combine shower validation plots')
    parser.add_argument('inputs', type=str, nargs='+', help="Name of the input files.")
    parser.add_argument('--output', "-o", dest='output', type=str, default="figures", help="Name of the output directory.")
    parser.add_argument("--histogramName","-n", help="Name of the plots to compare (default: all).", type = str, nargs='+')
    parser.add_argument('--visual', "-v", dest='visual', action='store_true', help="If plots should be also displayed.")
    parser.add_argument('--legend', "-l", dest='legend', type=str, nargs="+", default = [], help="Labels to be displayed in legend.")
    parser.add_argument('--legendShort', "-ls", dest='legendShort', type=str, nargs="+", default = [], help="Labels to be used as names of the histograms. If empty, filenames are used..")
    parser.add_argument('--noratio', "-r", dest='noratio', action='store_true', help="Do not plot ratio, just overlap plots.")
    args = parser.parse_args()
    if len(args.legend) == 0:
       legend_names = args.inputs
    elif len(args.legend) != len(args.inputs):
       sys.exit("Number of labels for legend ("+str(len(args.legend))+") must be equal to the number of input files ("+str(len(args.inputs))+").")
    else:
       legend_names = args.legend
    if len(args.legendShort) == 0:
       legend_short_names = args.inputs
    elif len(args.legendShort) != len(args.inputs):
       sys.exit("Number of labels for legend ("+str(len(args.legendShort))+") must be equal to the number of input files ("+str(len(args.inputs))+").")
    else:
       legend_short_names = args.legendShort
    if not args.visual:
       ROOT.gROOT.SetBatch(True)
    main(args.inputs, args.output, legend_names, legend_short_names, args.histogramName, args.noratio)
    if args.visual:
       input("Press ENTER to exit")
