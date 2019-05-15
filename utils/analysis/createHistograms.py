import os
import sys

if len(sys.argv) < 2:
    sys.exit("======= \nDefine list of input files as arguments.\n=======")

from ROOT import gSystem
gSystem.Load("libAnalysisDictionary.so")

from ROOT import createHistograms
for in_file in sys.argv[1:]:
    out_file = "./histograms_" + in_file.split("/")[-1]
    print "input file: ", in_file
    print "output file: ", out_file
    createHistograms(in_file, out_file)
