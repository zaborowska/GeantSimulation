#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "H5Cpp.h"
#include "H5File.h"
#include "H5DataSet.h"
#include "root2h5.h"

void root2h5(const std::string& aInput, const std::string& aOutput) {
  TFile f(aInput.c_str(), "READ");
  if (f.IsZombie()) {
    return;
  }
  const hsize_t netSize = 25;
  const int rank = 4;
  const hsize_t numEvents = static_cast<TTree*>(f.Get("events"))->GetEntries();
  const hsize_t storeMax = 100;
  const uint numFiles = floor(numEvents / storeMax);
  std::cout << "Get number of events: " << storeMax * numFiles << std::endl;

  std::cout << "Created dataset " << std::endl;
  int iE, i, j, k;
  float particles[storeMax];
  float data[storeMax][netSize][netSize][netSize];
  std::cout << "Created array " << std::endl;

  uint xCell = 0, yCell = 0, zCell = 0;
  double eCell = 0;
  uint iEvent = 0, iFile =0;
  TTree* events = nullptr;
  TTreeReader eventsReader("events",&f);
  TTreeReaderValue<double> energyMC(eventsReader, "EnergyMC");
  TTreeReaderValue<std::vector<double>> energyCellV(eventsReader, "EnergyCell");
  TTreeReaderValue<std::vector<int>> xCellV(eventsReader, "xCell");
  TTreeReaderValue<std::vector<int>> yCellV(eventsReader, "yCell");
  TTreeReaderValue<std::vector<int>> zCellV(eventsReader, "zCell");
  double sum = 0;

  const H5std_string h5OutputName( aOutput );
  const H5std_string h5DataName_cells( "events_cells" );
  const H5std_string h5DataName_particles( "events_particles" );
  hsize_t h5Dim_cells[4] = {storeMax, netSize, netSize, netSize};
  hsize_t h5Dim_cells_current[4] = {0, netSize, netSize, netSize};
  hsize_t h5Dim_cells_max[4] = {storeMax * numFiles, netSize, netSize, netSize};
  hsize_t h5Dim_particles[1] = {storeMax};
  hsize_t h5Dim_particles_current[1] = {0};
  hsize_t h5Dim_particles_max[1] = {storeMax * numFiles};
  H5::DSetCreatPropList h5cparms_cells;
  h5cparms_cells.setChunk( 4, h5Dim_cells );
  H5::DSetCreatPropList h5cparms_particles;
  h5cparms_particles.setChunk( 1, h5Dim_particles );
  H5::H5File h5OutFile( h5OutputName, H5F_ACC_TRUNC );
  H5::IntType datatype( H5::PredType::NATIVE_FLOAT );
  H5::DataSpace dataspace_cells( rank, h5Dim_cells_current, h5Dim_cells_max );
  H5::DataSet dataset_cells = h5OutFile.createDataSet( h5DataName_cells, datatype, dataspace_cells, h5cparms_cells );
  H5::DataSpace dataspace_particles( 1, h5Dim_particles_current, h5Dim_particles_max );
  H5::DataSet dataset_particles = h5OutFile.createDataSet( h5DataName_particles, datatype, dataspace_particles, h5cparms_particles );

  while(eventsReader.Next()){
    particles[iEvent] = *energyMC;
    sum = 0;
    for (j = 0; j < netSize; j++) {
      for (i = 0; i < netSize; i++) {
        for (k = 0; k < netSize; k++) {
          data[iEvent][j][i][k] = 0;
        }
      }
    }
    for (uint iEntry = 0; iEntry < energyCellV->size(); ++iEntry) {
      xCell = xCellV->at(iEntry);
      yCell = yCellV->at(iEntry);
      zCell = zCellV->at(iEntry);
      eCell = energyCellV->at(iEntry);
      sum += eCell;
      if(xCell < netSize && yCell < netSize && zCell < netSize ) {
        data[iEvent][xCell][yCell][zCell] = eCell;
      } else {
        std::cout << " ERROR, attempting to acces [" << xCell << "][" << yCell << "][" << zCell << std::endl;
        return;
      }
    }
    if ( sum > *energyMC ) {
      std::cout << " ERROR, unphysical sum of cells: " << sum << " > " << *energyMC << std::endl;
      return;
    }
    iEvent++;
    if (iEvent == storeMax) {
      iEvent = 0;
      hsize_t offset_cells[4] = {storeMax * iFile, 0,0,0};
      hsize_t offset_particles[1] = {storeMax * iFile};
      h5Dim_cells_current[0] += storeMax;
      h5Dim_particles_current[0] += storeMax;
      dataset_cells.extend(h5Dim_cells_current);
       H5::DataSpace *filespace_cells = new H5::DataSpace(dataset_cells.getSpace ());
       filespace_cells->selectHyperslab(H5S_SELECT_SET, h5Dim_cells, offset_cells);
       H5::DataSpace *memspace_cells = new H5::DataSpace(4, h5Dim_cells);
       dataset_cells.write(data, datatype, *memspace_cells, *filespace_cells);
      dataset_particles.extend(h5Dim_particles_current);
      H5::DataSpace *filespace_particles = new H5::DataSpace(dataset_particles.getSpace ());
      filespace_particles->selectHyperslab(H5S_SELECT_SET, h5Dim_particles, offset_particles);
      H5::DataSpace *memspace_particles = new H5::DataSpace(1, h5Dim_particles);
      dataset_particles.write(particles, datatype, *memspace_particles, *filespace_particles);

      iFile ++;
      if (iFile == storeMax ) {
        std::cout << "Drop remaining " << numEvents - numFiles * storeMax << " events." << std::endl;
        return;
      }
    }
  }
  h5OutFile.close();
  return;
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
                                  inputName.find(".root") - inputName.find_last_of("/") - 1) + ".h5" ;
    std::cout << "Using default output path: \"./" << outputName << "\"" << std::endl;
  } else {
    outputName = argv[2];
  }
  root2h5(inputName, outputName);


  return 0;
}
