import os
import sys

# first check if dictionary exists
if not os.path.isfile("libH5Dict.so"):
    sys.exit("======= \nRun \"make\" to create ROOT dictionary.\n=======")

if len(sys.argv) < 2:
    sys.exit("======= \nDefine list of input files as arguments.\n=======")

from ROOT import gSystem
gSystem.Load("libH5Dict.so")

from ROOT import h52root
for in_file in sys.argv[1:]:
    out_file = "./" + in_file.split("/")[-1].replace(".h5",".root")
    print "input file: ", in_file
    print "output file: ", out_file
    h52root(in_file, out_file)
