Analysis tool
===

Input: ROOT file that is the output of G4 simulation

TODO: read also h5 (or provide translation interface)

Output: ROOT file with histograms

TODO: [WIP] plot histograms in a more friendly way, combine different energies, compare baseline (full sim) with fast sim

How to run
==

To compile analysis tool:

```
make
```

Executable `createHistograms` s created together with ROOT dictionary `libCreateHistogramsDict.so`. To create histograms:

```
./createHistograms <INPUT_FILE> <OUTPUT_FILE>(optional: DEFAULT "validation_<INPUT_FILE>")
```

or

```
python createHistograms.py <LIST_OF_INPUT_FILES>
```