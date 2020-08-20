import os
import sys
import argparse

from ROOT import gSystem
gSystem.Load("libh52rootDictionary.so")
from ROOT import h52root

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='H5 to ROOT conversion tools')
    parser.add_argument('inputs', type=str, nargs='+', help="Names of the input files")
    parser.add_argument('--datasetCellsName', dest='datasetCellsName', type=str, default="events_cells", help="Name of the dataset containing cells' energy")
    group = parser.add_mutually_exclusive_group()
    group.add_argument('--datasetEnergyName', dest='datasetEnergyName', type=str, help="Name of the dataset containing MC particle energy (ignored if --energy is defined)")
    group.add_argument('--energy', '-e', dest='energy', type=int, help="Single value of MC particle energy (MeV)")
    parser.add_argument('--numCells', '-n', dest='numCells', type=int, default=24, help="Number of grid cells (equal in each direction)")
    parser.add_argument('--useCartesian', '-c', dest='useCartesian', action="store_true", help="If Cartesian coordinates are used instead of cylindrical")
    parser.add_argument('--energyThreshold', '-t', dest='energyThreshold', type=float, default=0, help="Minimal value of cell energy (values below are set to 0)")
    args = parser.parse_args()
    print("name of dataset containing cells' energy: ", args.datasetCellsName)
    if args.energy:
        datasetEnergyName = ""
        energy = args.energy
        print("use single MC particle energy: ", energy)
    else:
        datasetEnergyName = "events_particles"
        energy = 0
        print("name of dataset containing MC particle energy: ", datasetEnergyName)
    print("number of cells: ", args.numCells)
    print("use Cartesian coordinates instead of cylindrical: ", args.useCartesian)
    for in_file in args.inputs:
        out_file = "./" + in_file.split("/")[-1].replace(".h5",".root")
        print("input file: ", in_file)
        print("output file: ", out_file)
        h52root(in_file, out_file, args.datasetCellsName, datasetEnergyName, energy, args.numCells, args.useCartesian, args.energyThreshold)
