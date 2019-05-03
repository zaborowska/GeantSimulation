H5 tools
===

ROOT -> H5
==

  ROOT file with `TTree` "events" with branches:
  -- `EnergyMC` (`double`)
  -- `EnergyCell` (`std::vector<double>`)
  -- `xCell` (`std::vector<double>`)
  -- `yCell` (`std::vector<double>`)
  -- `zCell` (`std::vector<double>`)

```
h5c++ root2h5.cpp -o root2h5 `root-config --cflags --glibs`
./root2h5 <INPUT_FILE>
```


H5 -> ROOT
==

```
make
```

Executable `h52root` is created together with ROOT dictionary `libH52RootDict.so`. To translate `.h5` file:

```
  ./h52root <INPUT_FILE> <OUTPUT_FILE>(optional: DEFAULT .h5->.root)
```

or

```
python h52root.py <LIST_OF_INPUT_FILES>
```
