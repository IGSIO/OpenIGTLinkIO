project(igtlioLogic)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINKIO_LOGIC_EXPORT")

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(VTK
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

set(${KIT}_INCLUDE_DIRECTORIES
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(${KIT}_SRCS
  IGTLIOUtilities.h
  IGTLIOUtilities.cxx
  vtkIGTLIOObject.h
  vtkIGTLIOObject.cxx
  vtkIGTLIODeviceFactory.cxx
  vtkIGTLIODeviceFactory.h
  vtkIGTLIOCircularBuffer.cxx
  vtkIGTLIOCircularBuffer.h
  vtkIGTLIOConnector.cxx
  vtkIGTLIOConnector.h
  vtkIGTLIOSession.cxx
  vtkIGTLIOSession.h
  vtkIGTLIOLogic.cxx
  vtkIGTLIOLogic.h
  )

set(${KIT}_TARGET_LIBRARIES
  igtlioDevices
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  )

add_library(${KIT} ${${KIT}_SRCS})
target_link_libraries(${KIT} ${${KIT}_TARGET_LIBRARIES})
target_include_directories(${KIT} ${${KIT}_INCLUDE_DIRECTORIES})

include( GenerateExportHeader )
generate_export_header( ${KIT} EXPORT_FILE_NAME ${KIT}Export.h EXPORT_MACRO_NAME ${${KIT}_EXPORT_DIRECTIVE} )

