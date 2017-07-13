project(igtlioTesting)

find_package(VTK REQUIRED NO_MODULE
  COMPONENTS
    vtkIOImage
    vtkImagingMath
    vtkImagingCore
    vtkIOXMLParser
  )
include(${VTK_USE_FILE})

set(${PROJECT_NAME}_SRCS
  IGTLIOFixture.cxx
  IGTLIOFixture.h
  )

set(${PROJECT_NAME}_MOC_SRCS
  )
  
set(${PROJECT_NAME}_UI_SRCS
  )


set(${PROJECT_NAME}_INCLUDE_DIRECTORIES PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(${PROJECT_NAME}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  igtlioLogic
  igtlioTools
  )

macro(add_io_test test_name test_target source_files)
  add_executable(${test_target} ${source_files} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_MOC_SRCS})
  target_link_libraries(${test_target} PUBLIC ${${PROJECT_NAME}_TARGET_LIBRARIES})
  target_include_directories(${test_target} ${${PROJECT_NAME}_INCLUDE_DIRECTORIES})
  add_test(${test_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_target})
endmacro()

add_io_test("testClientServer" testClientServer testClientServer.cxx)
add_io_test("testReceiveImage" testReceiveImage testReceiveImage.cxx)
add_io_test("testSendReceiveCommand" testSendReceiveCommand testSendReceiveCommand.cxx)
add_io_test("testDeduceToolBasedOnName" testDeduceToolBasedOnName testDeduceToolBasedOnName.cxx)
add_io_test("testCommandMessageCodec" testCommandMessageCodec testCommandMessageCodec.cxx)
add_io_test("testSendReceiveCommandWidthCodec" testSendReceiveCommandWidthCodec testSendReceiveCommandWidthCodec.cxx)
add_io_test("testReceiveVideo" testReceiveVideo testReceiveVideo.cxx)