#include "myGVFlashHomoShowerTuning.hh"
#include "G4DataVector.hh"

myGVFlashHomoShowerTuning::myGVFlashHomoShowerTuning(const G4String& aName) {
  if( aName.empty() ) {
    G4cout << "Using default GFlash parameters." << G4endl;
  } else {
    readParameters(aName);
  }
}

void myGVFlashHomoShowerTuning::readParameters(const G4String& aName) {
  G4DataVector parameters(10);
  std::ifstream fin;
  fin.open(aName.c_str(),std::ifstream::in);
  if (fin.fail()) {
    G4Exception("myGVFlashHomoShowerTuning::readParameters()",
                "InvalidSetup", FatalException, ("File with parameters " + aName + " does not extst.").c_str());
  }
  std::cout << std::endl << "PARAMETERS read from " << aName << std::endl;
  std::cout << std::endl << "PARAMETERS length " << parameters.size() << std::endl;
  parameters.Retrieve(fin, true);
  std::cout << std::endl << "PARAMETERS" << std::endl;
  for (const auto& item: parameters) {
    std::cout << item << "\t";
  }
  std::cout << std::endl << std::endl;

  std::ifstream tmp("test.dat",std::ifstream::in);
  char c;
  while (tmp.get(c))          // loop getting single characters
    std::cout << c;
  std::cout<<std::endl;
  fParAveT1 = parameters[0];
  fParAveT2 = parameters[1];
  fParAveA1 = parameters[2];
  fParAveA2 = parameters[3];

  fParSigLogT1 = parameters[4];
  fParSigLogT2 = parameters[5];

  fParSigLogA1 = parameters[6];
  fParSigLogA2 = parameters[7];

  fParRho1 = parameters[8];
  fParRho2 = parameters[9];
}

void myGVFlashHomoShowerTuning::printParameters() const {
  G4cout << "\nGFlash parametrisation \n === Longitudinal profile \nT = " << fParAveT2 << " * log(y) + " << fParAveT1
         << "\nAlpha = " << fParAveA2 << " * log(y) + " << fParAveA1
         << "\nsigma(log T) = 1. / (" << fParSigLogT2 << " * log(y) + " << fParSigLogT1<< ")"
         << "\nsigma(log Alpha) = 1. / (" << fParSigLogA2 << " * log(y) + " << fParSigLogA1 << ")"
         << "\ncorr(log Alpha) = " << fParRho2 << " * log(y) + " << fParRho1<< G4endl;
}
