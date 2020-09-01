# UTILS

Utils for analysis of showers, and translation between ROOT and HDF5.

Can be installed separately from the Geant4 application, using `CMakeLists.txt` from this directory:

```
export PROJECT_DIR=`git rev-parse --show-toplevel`
mkdir ${PROJECT_DIR}/build ${PROJECT_DIR}/install
cd ${PROJECT_DIR}/build
cmake -DCMAKE_INSTALL_PREFIX=../install ../utils/
make install
```

[More details about analysis](analysis/README.md)

[More details about conversion tools](h5/README.md)