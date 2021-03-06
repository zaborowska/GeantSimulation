#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(sampleSimulation)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL ${CPP11FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_CXX_STANDARD 17)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
message(STATUS "Found Geant: ${Geant4_INCLUDE_DIR}")
#----------------------------------------------------------------------------

option(RICHVIS "Enter rich visualization mode" OFF)
if (RICHVIS)
  add_definitions(-DRICHVIS)
  message("Compile with additional features for richer visualisation")
endif()

option(REPRODUCESEEDS "Store and restore seeds for each event" OFF)
if (REPRODUCESEEDS)
  add_definitions(-DREPRODUCESEEDS)
  message("Compile with storing and restoring of seeds")
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include/
                    ${Geant4_INCLUDE_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc  ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(sampleSimulation ${sources} ${headers})
target_link_libraries(sampleSimulation ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS sampleSimulation DESTINATION bin)


#----------------------------------------------------------------------------
# Find ROOT (required package only for utils)
#
find_package(ROOT)
if (ROOT_FOUND)
   find_package(ROOT CONFIG REQUIRED RooFit RooFitCore)
   message(STATUS "Found ROOT: ${ROOT_DIR}, utils will be built")
   add_subdirectory(utils/analysis)
   add_subdirectory(utils/h5)
else()
   message(STATUS "ROOT was not found, utils will not be built")
endif()
