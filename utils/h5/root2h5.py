import os
import sys
import argparse

from ROOT import gSystem
gSystem.Load("libroot2h5Dictionary.so")
from ROOT import root2h5

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='ROOT to H5 conversion tools')
    parser.add_argument('inputs', type=str, nargs='+', help="Names of the input files")
    parser.add_argument('--numCells', '-n', dest='numCells', type=int, default=24, help="Number of grid cells (equal in each direction)")
    parser.add_argument('--useCartesian', '-c', dest='useCartesian', action="store_true", help="If Cartesian coordinates are used instead of cylindrical")
    args = parser.parse_args()
    print("number of cells: ", args.numCells)
    print("use Cartesian coordinates instead of cylindrical: ", args.useCartesian)
    for in_file in args.inputs:
        out_file = "./" + in_file.split("/")[-1].replace(".root",".h5")
        print("input file: ", in_file)
        print("output file: ", out_file)
        root2h5(in_file, out_file, args.numCells, args.useCartesian)
