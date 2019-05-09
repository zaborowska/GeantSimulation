Analysis tool
==

1. Analyse showers

Input: ROOT file that is the output of G4 simulation

Output: ROOT file with histograms


How to run
====

To compile analysis tool:

```
make
```

Executable `createHistograms` is created together with ROOT dictionary `libCreateHistogramsDict.so`. To create histograms:

```
./createHistograms <INPUT_FILE> <OUTPUT_FILE>(optional: DEFAULT "validation_<INPUT_FILE>")
```

or

```
python createHistograms.py <LIST_OF_INPUT_FILES>
```

2. Combine results


Combine results from several files (each file per one single energy value)

TODO: Implement merging of files if more than one file per energy value is provided

TODO: Allow also for flat energy spectrum to be combined

How to run
====

```
python combine.py <LIST_OF_INPUT_FILES> -o <OUTPUT_FILE>
```

3. Compare results


How to run
====

```
python compare.py <LIST_OF_INPUT_FILES> -o <OUTPUT_FILE>
```
