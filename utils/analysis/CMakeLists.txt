cmake_minimum_required (VERSION 3.2.2)
project (sampleSimulation_utils_analysis)
include_directories (${PROJECT_BINARY_DIR})
include("${ROOT_DIR}/modules/RootNewMacros.cmake")

set (tools
  createHistograms
  )
foreach (tool ${tools})
  add_executable (${tool} ${PROJECT_SOURCE_DIR}/${tool}.cpp)
  target_link_libraries(${tool} ${ROOT_LIBRARIES})
  install(TARGETS ${tool} DESTINATION bin)
  root_generate_dictionary(G_${tool} ${tool}.cpp LINKDEF ${PROJECT_SOURCE_DIR}/LinkDef.h)
  add_library(AnalysisDictionary SHARED ${tool} G_${tool}.cxx)
  target_link_libraries(AnalysisDictionary ${ROOT_LIBRARIES})
  install(TARGETS AnalysisDictionary DESTINATION lib)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libG_${tool}_rdict.pcm DESTINATION lib)
endforeach (tool ${tools})

