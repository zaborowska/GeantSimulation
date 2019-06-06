import os
import sys
import argparse

from ROOT import gSystem
gSystem.Load("libh52rootDictionary.so")
from ROOT import h52root

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='H5 to ROOT conversion tools')
    parser.add_argument('inputs', type=str, nargs='+', help="name of the input files")
    parser.add_argument('--datasetCellsName', dest='datasetCellsName', type=str, default="ECAL", help="Name of the dataset containing cells")
    parser.add_argument('--datasetEnergyName', dest='datasetEnergyName', type=str, default="", help="Name of the dataset containing MC energy")
    parser.add_argument('--datasetEnergyIndex', dest='datasetEnergyIndex', type=int, default=0, help="Index of the MC energy in the dataset containing MC energy")
    parser.add_argument('--energy', '-e', dest='energy', type=int, default=100, help="MC energy")
    parser.add_argument('--energyUnitToMeV', '-unit', dest='unit', type=float, default=1.e3, help="MC energy unit conversion to MeV")
    args = parser.parse_args()
    for in_file in args.inputs:
        out_file = "./" + in_file.split("/")[-1].replace(".h5",".root")
        print "input file: ", in_file
        print "output file: ", out_file
        h52root(in_file, out_file, args.datasetCellsName, args.datasetEnergyName, args.energy, args.datasetEnergyIndex, args.unit)
