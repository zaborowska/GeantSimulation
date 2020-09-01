# Analysis tool

Default units:
- energy: MeV
- length: mm
- time: s

## 1. Analyse showers

Create control histograms for shower (if stored in h5 file first [translate it](../h5/README.md#h5---root)).

Currently, analysis assumes cylindrical coordinates (to be extended) and that z axis is defined by the shower longitudinal axis.

### Input

ROOT file that is the output of G4 simulation. It contains `TTree` "events" with branches:

  - `EnergyMC` (`double`)
  - `EnergyCell` (`std::vector<double>`)
  - `rhoCell` (`std::vector<double>`)
  - `phiCell` (`std::vector<double>`)
  - `zCell` (`std::vector<double>`)

### Output

ROOT file with histograms.

### How to run

Executable `createHistograms` is created together with ROOT dictionary `libCreateHistogramsDict.so` (for use in python). To create histograms:

```
`git rev-parse --show-toplevel`/install/bin/createHistograms <INPUT_FILE> [<OUTPUT_FILE> optional]
```

or

```
export PROJECT_DIR=`git rev-parse --show-toplevel`
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PROJECT_DIR}/install/lib/ python ${PROJECT_DIR}/utils/analysis/createHistograms.py <LIST_OF_INPUT_FILES>
```

## 2. Combine results

Combine results (histograms) from several files (each file per one single energy value)

TODO: Implement merging of files if more than one file per energy value is provided

TODO: Allow also for flat energy spectrum to be combined


```
python `git rev-parse --show-toplevel`/utils/analysis/combine.py <LIST_OF_INPUT_FILES> [-o <OUTPUT_FILE> optional] [--visual optional]


positional arguments:
  inputs                name of the input files

optional arguments:
  --output OUTPUT, -o OUTPUT
                        name of the output file
  --visual, -v          If plots should be also displayed.

```

## 3. Compare results

Compare different samples, full/fast simulation, etc.


```
python  `git rev-parse --show-toplevel`/utils/analysis/compare.py <LIST_OF_INPUT_FILES> [-o <OUTPUT_FILE> optional]

positional arguments:
  inputs                Name of the input files.

optional arguments:
  --output OUTPUT, -o OUTPUT
                        Name of the output file.
  --histogramName HISTOGRAMNAME [HISTOGRAMNAME ...], -n HISTOGRAMNAME [HISTOGRAMNAME ...]
                        Name of the plots to compare (default: all).
  --visual, -v          If plots should be also displayed.
  --legend LEGEND [LEGEND ...], -l LEGEND [LEGEND ...]
                        Labels to be displayed in legend.

```
