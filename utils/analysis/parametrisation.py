import os
import sys
import argparse

from ROOT import gSystem
gSystem.Load("libparametrisationDictionary.so")
from ROOT import parametrisation

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create validation histograms')
    parser.add_argument('inputs', type=str, nargs='+', help="name of the input files")
    parser.add_argument('--material', "-m", required=True, type=str, help="Material to define X0, RM and Ec")
    parser.add_argument('--sizeInMm', "-size", required=True, type=float, nargs="+", help="Size of cell in mm [xy, z]")
    parser.add_argument('--numCells', "-num", required=True, type=int, nargs="+", help="Number of cells [xy, z]")
    args = parser.parse_args()

    if args.material == "Pb":
        X0 = 5.612
        RM = 16.02
        EC = 0.00743
    if args.material == "W":
        X0 = 3.504
        RM = 9.327
        EC = 0.00797
    if args.material == "PbWO4":
        X0 = 8.903
        RM = 16.02
        EC = 0.00964

    print ( " ===== " )
    print ( " == setting parameters == " )
    print ( " == X0 = " + str(X0) + " == " )
    print ( " == RM = " + str(RM) + "  == " )
    print ( " == EC = " + str(EC) + "  == " )
    print ( " ===== " )

    if len(args.sizeInMm) < 2:
        sys.exit("Size in [xy, z] is needed. Please pass two numbers.")
    if len(args.numCells) < 2:
        sys.exit("Number of cells in [xy, z] is needed. Please pass two numbers.")

    print ( " ===== " )
    print ( " == transverse = " + str(args.numCells[0]) + " x " + str(args.sizeInMm[0]) + " mm cells == " )
    print ( " == longitudinal = " + str(args.numCells[1]) + " x " + str(args.sizeInMm[1]) + " mm cells == " )
    print ( " ===== " )

    # ignore messages from RooFit that are still printed
    for in_file in args.inputs:
        out_file = "./parametrisation_" + in_file.split("/")[-1]
        print( "input file: ", in_file)
        print( "output file: ", out_file)
        parametrisation(in_file, out_file, X0, RM, EC, args.numCells[0], args.numCells[1], args.sizeInMm[0], args.sizeInMm[1])
