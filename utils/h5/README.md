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
