cmake_minimum_required (VERSION 3.2.2)
project (sampleSimulation_utils_h5)
find_package(HDF5 COMPONENTS CXX HL)
if(HDF5_FOUND)
  include_directories (${HDF5_INCLUDE_DIR}
                       ${ROOT_INCLUDE_DIRS}
                       ${PROJECT_BINARY_DIR})
  include("${ROOT_DIR}/RootMacros.cmake")

  set (tools
    h52root
    root2h5
  )
  foreach (tool ${tools})
    add_executable (${tool} ${PROJECT_SOURCE_DIR}/${tool}.cpp ${PROJECT_SOURCE_DIR}/${tool}.h)
    target_link_libraries(${tool} ${ROOT_LIBRARIES} -L${LIBDIR} ${HDF5_HL_LIBRARIES} ${HDF5_LIBRARIES})
    install(TARGETS ${tool} DESTINATION bin)
    root_generate_dictionary(G_${tool} ${PROJECT_SOURCE_DIR}/${tool}.h LINKDEF ${PROJECT_SOURCE_DIR}/LinkDef.h)
    add_library(${tool}Dictionary SHARED ${tool} G_${tool}.cxx)
    target_link_libraries(${tool}Dictionary ${ROOT_LIBRARIES} ${HDF5_HL_LIBRARIES} ${HDF5_LIBRARIES})
    install(TARGETS ${tool}Dictionary DESTINATION lib)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libG_${tool}_rdict.pcm DESTINATION lib)
  endforeach (tool ${tools})
else()
  message(STATUS "hdf5 not found, skipping h5 utils")
endif()
