import os
import sys

if len(sys.argv) < 2:
    sys.exit("======= \nDefine list of input files as arguments.\n=======")

from ROOT import gSystem
gSystem.Load("libroot2h5Dictionary.so")

from ROOT import root2h5
for in_file in sys.argv[1:]:
    out_file = "./" + in_file.split("/")[-1].replace(".root",".h5")
    print "input file: ", in_file
    print "output file: ", out_file
    root2h5(in_file, out_file)
