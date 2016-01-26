project(igtlioDevices)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINKIO_DEVICES_EXPORT")

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(VTK 6.1
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
  vtkIGTLIODevice.cxx
  vtkIGTLIODevice.h
  vtkIGTLIOImageDevice.cxx
  vtkIGTLIOImageDevice.h
  )

set(${KIT}_TARGET_LIBRARIES
  igtlioConverter
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  )

add_library(${KIT} ${${KIT}_SRCS})
target_link_libraries(${KIT} ${${KIT}_TARGET_LIBRARIES})
target_include_directories(${KIT} ${${KIT}_INCLUDE_DIRECTORIES})

include( GenerateExportHeader )
generate_export_header( ${KIT} EXPORT_FILE_NAME ${KIT}Export.h EXPORT_MACRO_NAME ${${KIT}_EXPORT_DIRECTIVE} )

