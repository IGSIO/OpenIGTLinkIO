project(igtlioTesting)

set(KIT "${PROJECT_NAME}")
    
find_package(VTK
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  vtkImagingCore
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

set(EXECUTABLE ${PROJECT_NAME})

set(${KIT}_SRCS
  IGTLIOFixture.cxx
  IGTLIOFixture.h
  )

set(${KIT}_MOC_SRCS
  )
  
set(${KIT}_UI_SRCS
)


set(${KIT}_INCLUDE_DIRECTORIES
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  igtlioLogic
  )

macro(add_io_test test_name test_target source_files)
  add_executable(${test_target} ${source_files} ${${KIT}_SRCS} ${${KIT}_MOC_SRCS})
  target_link_libraries(${test_target} PUBLIC ${${KIT}_TARGET_LIBRARIES})
  target_include_directories(${test_target} ${${KIT}_INCLUDE_DIRECTORIES})
  add_test(${test_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_target})
endmacro()

add_io_test("testClientServer" testClientServer testClientServer.cxx)
add_io_test("testReceiveImage" testReceiveImage testReceiveImage.cxx)
add_io_test("testSendReceiveCommand" testSendReceiveCommand testSendReceiveCommand.cxx)
