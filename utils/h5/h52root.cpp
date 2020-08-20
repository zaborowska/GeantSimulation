#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "H5Cpp.h"
#include "H5File.h"
#include "H5DataSet.h"
#include "h52root.h"

void h52root(const std::string& aInput, const std::string& aOutput, const std::string& aDatasetCells = "events_cells", const std::string& aDatasetEnergy = "", int aEnergyMC = 0, int aNumCells = 24, bool aUseCartesian = false, double aEnergyThreshold = 0) {
  std::cout << "Print settings:\n input name: " << aInput << "\n output name: " << aOutput
            << "\n dataset with cells: " << aDatasetCells;
  if (! aEnergyMC == 0) std::cout << "\n dataset with MC energy: " << aDatasetEnergy << std::endl;
  else std::cout << "\n MC particle energy: " << aEnergyMC << " MeV." << std::endl;
  TFile f(aOutput.c_str(), "RECREATE");

  // TODO Fix hardcoded size
  const hsize_t netSize = 24;
  const int rank = 4;
  const hsize_t storeMax = 100;
  const double energyThresholdToSave = aEnergyThreshold;

  float particles[storeMax];
  // TODO Fix hardcoded size
  float data[storeMax][netSize][netSize][netSize];

  TTree* events = new TTree("events", "events");
  double read_energyMC;
  std::vector<double> read_energyCellV;
  std::vector<int> read_xCellV;
  std::vector<int> read_yCellV;
  std::vector<int> read_zCellV;
  events->Branch("EnergyMC",&read_energyMC);
  events->Branch("EnergyCell",&read_energyCellV);
  std::string name1stAxis = "rhoCell";
  std::string name2ndAxis = "phiCell";
  if (aUseCartesian) {
    name1stAxis = "xCell";
    name2ndAxis = "yCell";
  }
  events->Branch(name1stAxis.c_str(),&read_xCellV);
  events->Branch(name2ndAxis.c_str(),&read_yCellV);
  events->Branch("zCell",&read_zCellV);

  uint iEvent = 0, i = 0, j = 0, k = 0;
  const H5std_string h5OutputName( aInput );
  const H5std_string h5DataName_cells( aDatasetCells );
  H5::H5File h5InFile( h5OutputName, H5F_ACC_RDONLY );
  H5::DataSet dataset_cells = h5InFile.openDataSet( h5DataName_cells);
  H5::DataSpace dataspace_cells = dataset_cells.getSpace();
  int rank_cells = dataspace_cells.getSimpleExtentNdims();
  assert(rank_cells == rank);
  hsize_t* dim_cells = new hsize_t[rank_cells];
  dataspace_cells.getSimpleExtentDims(dim_cells);
  assert(dim_cells[1] == netSize && dim_cells[2] == netSize && dim_cells[3] == netSize);
  hsize_t*  offset_cells = new hsize_t[rank_cells];
  hsize_t*  h5Dim_cells = new hsize_t[rank_cells];
  h5Dim_cells[0]  = storeMax;
  h5Dim_cells[1]  = dim_cells[1];
  h5Dim_cells[2]  = dim_cells[2];
  h5Dim_cells[3]  = dim_cells[3];
  offset_cells[1]  = 0;
  offset_cells[2]  = 0;
  offset_cells[3]  = 0;
  H5::DataSpace memspace_cells(rank_cells, h5Dim_cells);

  if ( aEnergyMC == 0) {
    const H5std_string h5DataName_particles( aDatasetEnergy );
    H5::DataSet dataset_particles = h5InFile.openDataSet( h5DataName_particles);
    H5::DataSpace dataspace_particles = dataset_particles.getSpace();
    int rank_particles = dataspace_particles.getSimpleExtentNdims();
    assert(rank_particles == 1);
    hsize_t* dim_particles = new hsize_t[rank_particles];
    dataspace_particles.getSimpleExtentDims(dim_particles);
    assert(dim_cells[0] == dim_particles[0]);
    hsize_t* offset_particles = new hsize_t[rank_particles];
    hsize_t* h5Dim_particles = new hsize_t[rank_particles];
    h5Dim_particles[0]  = storeMax;
    H5::DataSpace memspace_particles(rank_particles, h5Dim_particles);

    for (uint iSlab = 0; iSlab < dim_particles[0]/storeMax; ++iSlab) {
      offset_cells[0] = iSlab * storeMax;
      dataspace_cells.selectHyperslab( H5S_SELECT_SET, h5Dim_cells, offset_cells );
      dataset_cells.read( data, H5::PredType::NATIVE_FLOAT, memspace_cells, dataspace_cells );
      offset_particles[0] = iSlab * storeMax;
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
              // TODO Fix hardcoded size
              if(data[iEvent][i][j][k] > energyThresholdToSave) {
                read_xCellV.push_back(i);
                read_yCellV.push_back(j);
                read_zCellV.push_back(k);
                // TODO Fix hardcoded size
                read_energyCellV.push_back(data[iEvent][i][j][k]);
              }
            }
          }
        }
        events->Fill();
      }
    }
  } else {
    for (uint iSlab = 0; iSlab < dim_cells[0]/storeMax; ++iSlab) {
      offset_cells[0] = iSlab * storeMax;
      dataspace_cells.selectHyperslab( H5S_SELECT_SET, h5Dim_cells, offset_cells );
      dataset_cells.read( data, H5::PredType::NATIVE_FLOAT, memspace_cells, dataspace_cells );
      for (iEvent = 0; iEvent < storeMax; iEvent++) {
        read_xCellV.clear();
        read_yCellV.clear();
        read_zCellV.clear();
        read_energyCellV.clear();
        read_energyMC = aEnergyMC;
        for (i = 0; i < netSize; i++) {
          for (j = 0; j < netSize; j++) {
            for (k = 0; k < netSize; k++) {
              // TODO Fix hardcoded size
              if(data[iEvent][i][j][k] > energyThresholdToSave) {
                read_xCellV.push_back(i);
                read_yCellV.push_back(j);
                read_zCellV.push_back(k);
                // TODO Fix hardcoded size
                read_energyCellV.push_back(data[iEvent][i][j][k]);
              }
            }
          }
        }
        events->Fill();
      }
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
  std::string datasetCellsName = "events_cells";
  std::string datasetEnergyName = "events_particles";
  int energyMC = 0;
  int numCells = 24;
  bool useCartesian = false;
  double energyCutoff = 0;
  if (argc < 3) {
    outputName = inputName.substr(inputName.find_last_of("/") + 1,
                                  inputName.find(".h5") - inputName.find_last_of("/") - 1) + ".root" ;
    std::cout << "Using default output path: \"./" << outputName << "\"" << std::endl;
  } else {
    outputName = argv[2];
    std::cout << "Using output path: \"./" << outputName << "\"" << std::endl;
  }
  if (argc < 4) {
    std::cout << "Using default cells dataset name: " << datasetCellsName << std::endl;
  } else {
    datasetCellsName = argv[3];
    std::cout << "Using cells dataset name: " << datasetCellsName << std::endl;
  }
  if (argc < 5) {
    std::cout << "Using default single energy value for MC particle energy (MeV): " << energyMC << std::endl;
  } else {
    energyMC = std::stoi(argv[4]);
    if (argc < 6 || energyMC > 0) {
      std::cout << "Using single energy value for MC particle energy (MeV): " << energyMC << std::endl;
    } else {
      datasetEnergyName = argv[5];
      std::cout << "Using dataset for MC particle energy: " << datasetEnergyName << std::endl;
    }
  }
  if (argc < 7) {
    numCells = 24;
    std::cout << "Using default number of cells (in each dimension): " << numCells << std::endl;
  } else {
    numCells = std::stoi(argv[6]);
    std::cout << "Using number of cells (in each dimension): " << numCells << std::endl;
  }
  if (argc < 8) {
    useCartesian = false;
    std::cout << "Using default cylindrical coordinates."<< std::endl;
  } else {
    useCartesian = std::stoi(argv[7]);
    if(useCartesian)
      std::cout << "Using Cartesian coordinates." << std::endl;
    else
      std::cout << "Using cylindrical coordinates." << std::endl;
  }
  if (argc < 9) {
    std::cout << "Saving by default all cell energy values (no threshold)."<< std::endl;
  } else {
    energyCutoff = std::stod(argv[8]);
    if (energyCutoff > 0)
      std::cout << "Saving cell energy only if above " << energyCutoff << " MeV." << std::endl;
    else
      std::cout << "Saving all cell energy values (no threshold)." << std::endl;
  }
  h52root(inputName, outputName, datasetCellsName, datasetEnergyName, energyMC, numCells, useCartesian, energyCutoff);
  return 0;
}
