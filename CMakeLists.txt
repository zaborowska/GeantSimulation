#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(geantDD4hep)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
# Set up C++11
set(CPP11FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL ${CPP11FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")


#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED)

# find_package( Boost REQUIRED)
 # include_directories( ${Boost_INCLUDE_DIRS} )

# find_package(podio REQUIRED HINTS $ENV{PODIO})
# message(STATUS "Found podio: ${podio_DIR}")
# link_directories(${podio_LIBRARY_DIR})
# message(STATUS "Found podio lib: ${PODIOLIB}")
# message(STATUS "Found podio include dir: ${podio_INCLUDE_DIRS}")
# Not the case if LD_LIBRARY_PATH is wrong
# find_library(PODIOLIB podio)
# if (NOT PODIOLIB)
   #  message(FATAL_ERROR "libpodio.so(dylib) cannot be found dynamically. Make sure you #have sourced PODIO init*.sh file to set up your environment to use PODIO")
# endif()

# find_package(fccedm REQUIRED HINTS $ENV{FCCEDM})
# message(STATUS "Found fcc-edm: ${fccedm_DIR}")
# message(STATUS "Found fcc-edm lib: ${FCCEDM_LIBRARIES}")
# message(STATUS "Found fcc-edm include dir: ${FCCEDM_INCLUDE_DIRS}")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include/
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS})
                    # ${podio_INCLUDE_DIRS}
                    # ${FCCEDM_INCLUDE_DIRS}
			
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc  ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(geantDD4hep ${sources} ${headers})
target_link_libraries(geantDD4hep ${Geant4_LIBRARIES}
                                 ${ROOT_LIBRARIES})
#                                utilities datamodel datamodelDict podio)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS geantDD4hep DESTINATION bin)
