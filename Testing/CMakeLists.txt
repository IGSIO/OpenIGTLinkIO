project(igtlioTesting)

set(KIT "${PROJECT_NAME}")

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
    
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

add_executable(testClientServer testClientServer.cxx ${${KIT}_SRCS} ${${KIT}_MOC_SRCS})
target_link_libraries(testClientServer PUBLIC ${${KIT}_TARGET_LIBRARIES})
target_include_directories(testClientServer ${${KIT}_INCLUDE_DIRECTORIES})
add_test("testClientServer" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testClientServer)

add_executable(testReceiveImage testReceiveImage.cxx ${${KIT}_SRCS} ${${KIT}_MOC_SRCS})
target_link_libraries(testReceiveImage PUBLIC ${${KIT}_TARGET_LIBRARIES})
target_include_directories(testReceiveImage ${${KIT}_INCLUDE_DIRECTORIES})
add_test("testReceiveImage" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testReceiveImage)
