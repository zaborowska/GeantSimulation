import ROOT
import sys

f = ROOT.TFile(sys.argv[1], "READ")
hist = ROOT.TH1F("hist", "MC particle energy", 100, 0, 1000)
for tree in f.GetListOfKeys():
    hist.Fill(float(tree.GetName()[2:])/1000)

hist.Draw()
raw_input('Press enter')
