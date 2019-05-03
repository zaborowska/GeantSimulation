import os
import sys

# first check if dictionary exists
if not os.path.isfile("libCreateHistogramsDict.so"):
    sys.exit("======= \nRun \"make\" to create ROOT dictionary.\n=======")

if len(sys.argv) < 2:
    sys.exit("======= \nDefine list of input files as arguments.\n=======")

from ROOT import gSystem
gSystem.Load("libCreateHistogramsDict.so")

from ROOT import validationPlots
for in_file in sys.argv[1:]:
    out_file = "./validation_" + in_file.split("/")[-1]
    print "input file: ", in_file
    print "output file: ", out_file
    validationPlots(in_file, out_file)
