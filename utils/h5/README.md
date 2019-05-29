# H5 tools

## ROOT -> H5

  ROOT file with `TTree` "events" with branches:

  - `EnergyMC` (`double`)
  - `EnergyCell` (`std::vector<double>`)
  - `xCell` (`std::vector<double>`)
  - `yCell` (`std::vector<double>`)
  - `zCell` (`std::vector<double>`)

```
${PROJECT_DIR}/install/bin/root2h5 <INPUT_FILE> [<OUTPUT_FILE> optional]
```

or

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PROJECT_DIR}/install/lib/ python ${PROJECT_DIR}/utils/h5/h52root.py <LIST_OF_INPUT_FILES>
```

Default name of the output file is created by changing the extension to `.h5` (from `.root`).


## H5 -> ROOT

H5 file with two datasets:
  - cells' energy (Nx25x25x25)
  - MC particles' properties (Nxk)


Executable `h52root` is created together with ROOT dictionary `libH52RootDict.so`. To translate `.h5` file:

```
${PROJECT_DIR}/install/bin/h52root
        <INPUT_FILE>
        [<OUTPUT_FILE>, default: <INPUT_FILE>(.root->.h5)]
        [<CELL_ENERGY_DATASET_NAME>, default: "ECAL"]
        [<MC_ENERGY_DATASET_NAME>, default: ""]
        [<MC_ENERGY_VALUE>, default: 100]
        [<MC_ENERGY_DATASET_INDEX>, default: 0]
        [<ENERGY_UNIT_TO_MEV>, default: 1]
```

or

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PROJECT_DIR}/install/lib/ python ${PROJECT_DIR}/utils/h5/h52root.py <LIST_OF_INPUT_FILES>
```

usage: h52root.py [-h] [--datasetCellsName DATASETCELLSNAME]
                  [--datasetEnergyName DATASETENERGYNAME]
                  [--datasetEnergyIndex DATASETENERGYINDEX] [--energy ENERGY]
                  [--energyUnitToMeV UNIT]
                  inputs [inputs ...]


1. Default name of the output file is created by changing the extension to `.root` (from `.h5`).
2. Name of the cell energy dataset is by dafault "ECAL" (`--datasetCellsName`).
3. If name of the MC energy dataset is not specified (`--datasetEnergyName`), it is assumed that all particles have same energy (`--energy` 100 GeV by default).
4. If name of the MC energy dataset is not empty (`--datasetEnergyName`) (and energy value `--energy` is set to 0), dataset is read and energy is taken as (`--datasetEnergyIndex` 0 == 1st by default) element in the row.
5. Conversion of units of MC energy and cell energy to MeV can be performed (`--energyUnitToMeV`).

Examples:
SiW training data conversion:
```
python utils/h5/h52root.py EleEscan_1_1.h5 --datasetEnergyName "target" --energy 0 --datasetEnergyIndex 1 -unit 1000.
```

SiW validation of generated single-energy showers:
```
python utils/h5/h52root.py e100GeV_1000events.h5 --datasetEnergyName "" --energy 100 -unit 1000.
```

