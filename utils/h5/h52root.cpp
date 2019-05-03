#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "H5Cpp.h"
#include "H5File.h"
#include "H5DataSet.h"

void h52root(const std::string& aInput, const std::string& aOutput) {

  TFile f(aOutput.c_str(), "RECREATE");

  const hsize_t netSize = 25;
  const int rank = 4;
  const hsize_t storeMax = 100;
  const double energyThresholdToSave = 0.1;

  float particles[storeMax];
  float data[storeMax][netSize][netSize][netSize];

  TTree* events = new TTree("events", "events");
  double read_energyMC;
  std::vector<double> read_energyCellV;
  std::vector<int> read_xCellV;
  std::vector<int> read_yCellV;
  std::vector<int> read_zCellV;
  events->Branch("EnergyMC",&read_energyMC);
  events->Branch("EnergyCell",&read_energyCellV);
  events->Branch("xCell",&read_xCellV);
  events->Branch("yCell",&read_yCellV);
  events->Branch("zCell",&read_zCellV);

  uint iEvent = 0, i = 0, j = 0, k = 0;
  const H5std_string h5OutputName( aInput );
  const H5std_string h5DataName_cells( "events_cells" );
  const H5std_string h5DataName_particles( "events_particles" );
  H5::H5File h5InFile( h5OutputName, H5F_ACC_RDONLY );
  H5::DataSet dataset_cells = h5InFile.openDataSet( h5DataName_cells);
  H5::DataSet dataset_particles = h5InFile.openDataSet( h5DataName_particles);
  H5::DataSpace dataspace_cells = dataset_cells.getSpace();
  H5::DataSpace dataspace_particles = dataset_particles.getSpace();
  int rank_cells = dataspace_cells.getSimpleExtentNdims();
  assert(rank_cells == rank);
  int rank_particles = dataspace_particles.getSimpleExtentNdims();
  assert(rank_particles == 1);
  hsize_t dim_cells[rank_cells];
  dataspace_cells.getSimpleExtentDims(dim_cells);
  hsize_t dim_particles[rank_particles];
  dataspace_particles.getSimpleExtentDims(dim_particles);
  assert(dim_cells[0] == dim_particles[0]);
  assert(dim_cells[1] == netSize && dim_cells[2] == netSize && dim_cells[3] == netSize);
  hsize_t  offset_cells[rank_cells];
  hsize_t  h5Dim_cells[rank_cells];
  h5Dim_cells[0]  = storeMax;
  h5Dim_cells[1]  = dim_cells[1];
  h5Dim_cells[2]  = dim_cells[2];
  h5Dim_cells[3]  = dim_cells[3];
  offset_cells[1]  = 0;
  offset_cells[2]  = 0;
  offset_cells[3]  = 0;
  hsize_t  offset_particles[rank_particles];
  hsize_t  h5Dim_particles[rank_particles];
  h5Dim_particles[0]  = storeMax;
  H5::DataSpace memspace_cells(rank_cells, h5Dim_cells);
  H5::DataSpace memspace_particles(rank_particles, h5Dim_particles);

  for (uint iSlab = 0; iSlab < dim_particles[0]/storeMax; ++iSlab) {
    offset_cells[0] = iSlab * storeMax;
    offset_particles[0] = iSlab * storeMax;
    dataspace_cells.selectHyperslab( H5S_SELECT_SET, h5Dim_cells, offset_cells );
    dataset_cells.read( data, H5::PredType::NATIVE_FLOAT, memspace_cells, dataspace_cells );
    dataspace_particles.selectHyperslab( H5S_SELECT_SET, h5Dim_particles, offset_particles );
    dataset_particles.read( particles, H5::PredType::NATIVE_FLOAT, memspace_particles, dataspace_particles );
    for (iEvent = 0; iEvent < storeMax; iEvent++) {
      read_xCellV.clear();
      read_yCellV.clear();
      read_zCellV.clear();
      read_energyCellV.clear();
      read_energyMC = particles[iEvent];
      for (i = 0; i < netSize; i++) {
        for (j = 0; j < netSize; j++) {
          for (k = 0; k < netSize; k++) {
            if(data[iEvent][i][j][k] > energyThresholdToSave) {
              read_xCellV.push_back(i);
              read_yCellV.push_back(j);
              read_zCellV.push_back(k);
              read_energyCellV.push_back(data[iEvent][i][j][k]);
            }
          }
        }
      }
      events->Fill();
    }
  }
  events->Write();
  f.Close();
}

int main(int argc, char** argv){
  if (argc < 2) {
    std::cout << "Please specify path to the input file." << std::endl;
    return -1;
  }
  std::string inputName = argv[1];
  std::string outputName = "";
  if (argc < 3) {
    outputName = inputName.substr(inputName.find_last_of("/") + 1,
                                  inputName.find(".h5") - inputName.find_last_of("/") - 1) + ".root" ;
    std::cout << "Using default output path: \"./" << outputName << "\"" << std::endl;
  } else {
    outputName = argv[2];
  }
  h52root(inputName, outputName);
  return 0;
}
