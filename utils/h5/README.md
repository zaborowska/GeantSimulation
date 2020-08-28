# H5 tools

## ROOT to H5 conversion

  ROOT file with `TTree` "events" with branches:

  - `EnergyMC` (`double`)
  - `EnergyCell` (`std::vector<double>`)
  - `rhoCell` or `xCell` (`std::vector<double>`)
  - `phiCell` or `yCell` (`std::vector<double>`)
  - `zCell` (`std::vector<double>`)

Cylindrical coordinates (`rhoCell` and `phiCell`) or Cartesian (`xCell` and `yCell`) can be used. Cylindrical coordinates are the default setting.

Number of cells in R x Phi x Z (or X x Y x Z for Cartesian coordinates) is assumed to be the same in all directions, and by default equal to 24.

Default name of the output file is created by changing the extension to `.h5` (from `.root`).

### To run

```
${PROJECT_DIR}/install/bin/root2h5 <INPUT_FILE> [<OUTPUT_FILE> optional] [<NUM_CELLS> optional] [<CARTESIAN(0/1)> optional]
```

where:
 - <OUTPUT_FILE> overwrites the default output name (input file name with the extension changed from `.root` to `.h5`)
 - <NUM_CELLS> sets number of cells in R x Phi x Z (or X x Y x Z for Cartesian coordinates). By default equal to 24.
 - <CARTESIAN> 0/1 flag specifying if Cartesian coordinates should be used instead of cylindrical (default) ones.

or

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PROJECT_DIR}/install/lib/ python ${PROJECT_DIR}/utils/h5/root2h5.py [--numCells NUMCELLS] [--useCartesian] inputs [inputs ...]
```

List of options for python script:
```
positional arguments:
  inputs                Names of the input files

optional arguments:
  --numCells NUMCELLS, -n NUMCELLS
                        Number of grid cells (equal in each direction)
  --useCartesian, -c    If Cartesian coordinates are used instead of cylindrical

```
Output names are created by changing the input file extension from `.root` to `.h5`.


## H5 to ROOT conversion

Executable `h52root` is created together with ROOT dictionary `libH52RootDict.so`. To translate `.h5` file:

```
${PROJECT_DIR}/install/bin/h52root <INPUT_FILE> [<OUTPUT_FILE> optional] [<DATASET_CELLS> optional] [<DATASET_PARTICLE> optional] [<ENERGY_PARTICLE> optional] [<NUM_CELLS> optional] [<CARTESIAN(0/1)> optional]
```

where:
 - <OUTPUT_FILE> overwrites the default output name (input file name with the extension changed from `.h5` to `.root`)
 - <DATASET_CELLS> specifies the name of the dataset containing Nxnxnxn values of cell energies, where N is the number of events, and n is the number of cells in a grid (default: "events_cells")
 - <ENERGY_PARTICLE> sets the single MC particle energy values, to be used for all events (default: 0). If set to larger than 0, name of <DATASET_PARTICLE> is ignored.
 - <DATASET_PARTICLE> specifies the name of the dataset containing N values of MC particle energies, where N is the number of events (default: "events_particles").
 - <NUM_CELLS> sets number of cells in R x Phi x Z (or X x Y x Z for Cartesian coordinates). By default equal to 24.
 - <CARTESIAN> 0/1 flag specifying if Cartesian coordinates should be used instead of cylindrical (default) ones.

or

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PROJECT_DIR}/install/lib/ python ${PROJECT_DIR}/utils/h5/h52root.py [--datasetCellsName DATASETCELLSNAME] [--datasetEnergyName DATASETENERGYNAME | --energy ENERGY] [--numCells NUMCELLS] [--useCartesian] inputs [inputs ...]
```

List of options for python script:
```
positional arguments:
  inputs                Names of the input files

optional arguments:
  -h, --help            show this help message and exit
  --datasetCellsName DATASETCELLSNAME
                        Name of the dataset containing cells' energy
  --datasetEnergyName DATASETENERGYNAME
                        Name of the dataset containing MC particle energy (ignored if --energy is defined)
  --energy ENERGY, -e ENERGY
                        Single value of MC particle energy (MeV)
  --numCells NUMCELLS, -n NUMCELLS
                        Number of grid cells (equal in each direction)
  --useCartesian, -c    If Cartesian coordinates are used instead of cylindrical
  --energyThreshold ENERGYTHRESHOLD, -t ENERGYTHRESHOLD
                        Minimal value of cell energy (values below are set to 0)

```

Default name of the output file is created by changing the extension to `.h5` (from `.root`).