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

import ROOT
import sys
ROOT.gStyle.SetOptStat(0000)
file = ROOT.TFile(sys.argv[1],"READ")

import os
print("Save output to :", sys.argv[1][:-5])
os.system("mkdir -p " +sys.argv[1][:-5])

# originaly used paramteres (input to GFlash)
if len(sys.argv) > 2:
   fileTxtName = sys.argv[2]
else:
   fileTxtName = "../data/fullsim/pbwo4_1k/data/combinedG4_PbWO4_individual.dat"
fileTxt = open(fileTxtName, 'r')
inputGflashParams = fileTxt.readlines()
EMC = file.Get("enMC").GetMean()
EC = 0.00964
from math import log
logY = log(EMC/EC)
meanLogT = log(float(inputGflashParams[2]) * logY + float(inputGflashParams[1]))
meanLogAlpha = log(float(inputGflashParams[4]) * logY + float(inputGflashParams[3]))
sigmaLogT = 1. / (float(inputGflashParams[6]) * logY + float(inputGflashParams[5]))
sigmaLogAlpha = 1. / (float(inputGflashParams[8]) * logY + float(inputGflashParams[7]))
rhoLogAlphaLogT =  (float(inputGflashParams[10]) * logY + float(inputGflashParams[9]))

hT = file.Get("longProfileT")
hBeta = file.Get("longProfileBeta")
hAlpha = file.Get("longProfileAlpha")
hLogT = file.Get("longProfileLogT")
hLogAlpha = file.Get("longProfileLogAlpha")
hTGen = file.Get("g4generatedLongProfileT")
hBetaGen = file.Get("g4generatedLongProfileBeta")
hAlphaGen = file.Get("g4generatedLongProfileAlpha")
hists=[hT, hAlpha, hBeta, hLogT, hLogAlpha]
histsGen = [hTGen, hAlphaGen, hBetaGen]
canvases=[]
hLogTMean = file.Get("g4generatedLongProfileLogTMean")
hLogTSigma = file.Get("g4generatedLongProfileLogTSigma")
hLogAlphaMean = file.Get("g4generatedLongProfileLogAlphaMean")
hLogAlphaSigma = file.Get("g4generatedLongProfileLogAlphaSigma")
fLogTGen = ROOT.TF1("fLogTGen","gaus",hLogT.GetXaxis().GetXmin(),hLogT.GetXaxis().GetXmax())
fLogTGen.FixParameter(1,hLogTMean.GetMean())
fLogTGen.FixParameter(2,hLogTSigma.GetMean())
fLogTInput = ROOT.TF1("fLogTInput", "gaus",hLogT.GetXaxis().GetXmin(),hLogT.GetXaxis().GetXmax())
fLogTInput.FixParameter(1,meanLogT)
fLogTInput.FixParameter(2,sigmaLogT)
fLogAlphaGen = ROOT.TF1("logAlphaGen","gaus",hLogAlpha.GetXaxis().GetXmin(),hLogAlpha.GetXaxis().GetXmax())
fLogAlphaGen.FixParameter(1,hLogAlphaMean.GetMean())
fLogAlphaGen.FixParameter(2,hLogAlphaSigma.GetMean())
fLogAlphaInput = ROOT.TF1("logAlphaInput", "gaus",hLogAlpha.GetXaxis().GetXmin(),hLogAlpha.GetXaxis().GetXmax())
fLogAlphaInput.FixParameter(1,meanLogAlpha)
fLogAlphaInput.FixParameter(2,sigmaLogAlpha)
print("mean T = (gen) ", hLogTMean.GetMean(), "   (input) ", meanLogT)
print("sigma T = (gen) ", hLogTSigma.GetMean(), "   (input) ", sigmaLogT)
print("mean alpha = (gen) ", hLogAlphaMean.GetMean(), "   (input) ", meanLogAlpha)
print("sigma alpha = (gen) ", hLogAlphaSigma.GetMean(), "   (input) ", sigmaLogAlpha)
fitfuns=[fLogTGen, fLogAlphaGen]
fitfunsInput=[fLogTInput, fLogAlphaInput]
fitfunText = ["G4 calc: log(T) = N("+str(round(hLogTMean.GetMean(),4))+", "+str(round(hLogTSigma.GetMean(),4))+"^{2})",
              "G4 calc: log(#alpha) = N("+str(round(hLogAlphaMean.GetMean(),4))+", "+str(round(hLogAlphaSigma.GetMean(),4))+"^{2})"]
fitfunInputText = ["G4 input: log(T) = N("+str(round(meanLogT,4))+", "+str(round(sigmaLogT,2))+"^{2})",
              "G4 input: log(#alpha) = N("+str(round(meanLogAlpha,4))+", "+str(round(sigmaLogAlpha,2))+"^{2})"]
for ih, h in enumerate(hists):
    h.SetMarkerColor(ROOT.kRed+1)
    h.SetLineColor(ROOT.kRed+1)
    h.SetFillColor(ROOT.kRed-9)
    h.SetFillStyle(3001)
    canvases.append(ROOT.TCanvas("c"+h.GetName(),h.GetName(),1200,800))
    canvases[ih].SetRightMargin(0.01)
    h.GetXaxis().SetRangeUser(h.GetMean() - 3 * h.GetRMS(),
                              h.GetMean() + 3 * h.GetRMS())
    h.Draw("epbar")
    legend = ROOT.TLegend(0.6,0.7,0.9,0.9)
    prepare_legend(legend)
    legend.AddEntry(h,"deposited energy","lep")
    if ih < len(histsGen):
        histsGen[ih].SetMarkerColor(9)
        histsGen[ih].SetLineColor(9)
        histsGen[ih].SetFillColor(ROOT.kBlue+4)
        histsGen[ih].SetFillStyle(3004)
        histsGen[ih].Draw("epbarsame")
        legend.AddEntry(histsGen[ih],"G4 generated parameters","lep")
    else:
        print("id = ",ih - len(histsGen))
        fitfuns[ih - len(histsGen)].SetLineColor(9)
        fitfuns[ih - len(histsGen)].SetLineWidth(2)
        fitfunsInput[ih - len(histsGen)].SetLineColor(ROOT.kGreen-4)
        fitfunsInput[ih - len(histsGen)].SetLineWidth(1)
        h.Fit(fitfuns[ih - len(histsGen)],"B+")
        h.Fit(fitfunsInput[ih - len(histsGen)],"B+")
        legend.AddEntry(fitfuns[ih - len(histsGen)], fitfunText[ih - len(histsGen)],"l")
        legend.AddEntry(fitfunsInput[ih - len(histsGen)], fitfunInputText[ih - len(histsGen)],"l")
    legend.Draw()
    canvases[ih].Update()
    canvases[ih].SaveAs(sys.argv[1][:-5]+"/"+canvases[ih].GetName()+".pdf")
    output=ROOT.TFile(sys.argv[1][:-5]+"/"+canvases[ih].GetName()+".root","RECREATE")
    output.cd()
    h.Write()
    if ih < len(histsGen):
        histsGen[ih].Write()
    else:
        fitfuns[ih - len(histsGen)].Write()
input("")
