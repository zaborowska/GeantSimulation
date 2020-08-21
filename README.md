# Geant4 application

Used to produce data for ML training, GFlash parametrisation, and validation of fast simulation.

```
mkdir build
cd build
cmake ..
make
```

If ROOT package is found, utils will be built (analysis, plotting, conversion to/from H5). They can be used as a standalone tool, [see more details](utils/README.md).