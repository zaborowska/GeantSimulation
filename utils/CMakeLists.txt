#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(showerValidation)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DDROP_CGAL -Wall -Wextra -Wpedantic -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_CXX_STANDARD 17)

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT CONFIG REQUIRED RooFit RooFitCore)
message(STATUS "Found ROOT: ${ROOT_DIR}")
include_directories(${ROOT_INCLUDE_DIRS})

add_subdirectory(analysis)
add_subdirectory(h5)
