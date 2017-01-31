project(igtlioDevices)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINKIO_DEVICES_EXPORT")

find_package(VTK
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

set(${KIT}_INCLUDE_DIRECTORIES
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${OpenIGTLinkIO_INCLUDE_INSTALL}>
  )

set(${KIT}_SRCS
  vtkIGTLIODevice.cxx
  vtkIGTLIODevice.h
  vtkIGTLIOImageDevice.cxx
  vtkIGTLIOImageDevice.h
  vtkIGTLIOStatusDevice.cxx
  vtkIGTLIOStatusDevice.h
  vtkIGTLIOCommandDevice.cxx
  vtkIGTLIOCommandDevice.h
  vtkIGTLIOTransformDevice.cxx
  vtkIGTLIOTransformDevice.h
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

INSTALL(TARGETS ${KIT} EXPORT OpenIGTLinkIO 
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}"
  LIBRARY ARCHIVE DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}"
  )

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}" PATTERN *.h)

INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/${KIT}Export.h" DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}")