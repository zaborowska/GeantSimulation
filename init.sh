#!/bin/sh -u
if [ -z "$BUILDTYPE" ] || [ "$BUILDTYPE" == "Release" ]; then
    export BINARY_TAG=x86_64-slc6-gcc49-opt
    export BUILDTYPE="Release"
else
    export BINARY_TAG=x86_64-slc6-gcc49-dbg
    export BUILDTYPE="Debug"
fi

source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r5p7/InstallArea/scripts/LbLogin.sh --cmtconfig $BINARY_TAG
# The LbLogin sets VERBOSE to 1 which increases the compilation output. If you want details et this to 1 by hand.
export VERBOSE=

export FCCEDM=/afs/cern.ch/exp/fcc/sw/0.7/fcc-edm/0.3/$BINARY_TAG/
export PODIO=/afs/cern.ch/exp/fcc/sw/0.7/podio/0.3/$BINARY_TAG

export CMAKE_PREFIX_PATH=$FCCEDM:$PODIO:/afs/cern.ch/sw/lcg/releases/LCG_83:$CMAKE_PREFIX_PATH:

export PYTHONDIR=/afs/cern.ch/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc48-opt
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PYTHONDIR/lib:$FCCEDM/lib:$PODIO/lib
export PYTHONPATH=$PYTHONPATH:$PODIO/lib:$PODIO/python:$FCCEDM/lib
export BOOST_ROOT=/afs/cern.ch/sw/lcg/external/Boost/1.55.0_python2.7/x86_64-slc6-gcc47-opt/

source /afs/cern.ch/sw/lcg/releases/LCG_83/ROOT/6.06.00/$BINARY_TAG/bin/thisroot.sh
source /afs/cern.ch/sw/lcg/releases/LCG_83/Geant4/10.02/$BINARY_TAG/Geant4-env.sh

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.2/setup_g4datasets.sh
export CMTPROJECTPATH=/afs/cern.ch/exp/fcc/sw/0.7/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh
# let ROOT know about the location of headers
export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$PODIO/include/datamodel:$FCCEDM/include/datamodel
