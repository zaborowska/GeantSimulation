import os
import sys
import argparse

from ROOT import gSystem
gSystem.Load("libAnalysisDictionary.so")
from ROOT import createHistograms

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create validation histograms')
    parser.add_argument('inputs', type=str, nargs='+', help="name of the input files")
    parser.add_argument('--minEnergy', '-min', dest='minEnergy', type=float, default=90, help="Minimal energy used for binning")
    parser.add_argument('--maxEnergy', '-max', dest='maxEnergy', type=float, default=110, help="Maximal energy used for binning")
    args = parser.parse_args()

    for in_file in args.inputs:
        out_file = "./histograms_" + in_file.split("/")[-1]
        print "input file: ", in_file
        print "output file: ", out_file
        createHistograms(in_file, out_file, args.minEnergy, args.maxEnergy)
