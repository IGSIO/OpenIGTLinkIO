project(igtlioConverter)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINKIO_CONVERTER_EXPORT")
    
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
  igtlioBaseConverter.cxx
  igtlioImageConverter.cxx
  igtlioPolyDataConverter.cxx
  igtlioStatusConverter.cxx
  igtlioTransformConverter.cxx
  igtlioCommandConverter.cxx
  )

set(${KIT}_HDRS
  igtlioBaseConverter.h
  igtlioImageConverter.h
  igtlioPolyDataConverter.h
  igtlioStatusConverter.h
  igtlioTransformConverter.h
  igtlioCommandConverter.h
  )

set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  )

add_library(${KIT} ${${KIT}_SRCS} ${${KIT}_HDRS})
target_link_libraries(${KIT} PUBLIC ${${KIT}_TARGET_LIBRARIES})
target_include_directories(${KIT} ${${KIT}_INCLUDE_DIRECTORIES})

include(GenerateExportHeader)
generate_export_header( ${KIT} EXPORT_FILE_NAME ${KIT}Export.h 
  EXPORT_MACRO_NAME ${${KIT}_EXPORT_DIRECTIVE} 
  )

INSTALL(TARGETS ${KIT} EXPORT OpenIGTLinkIO 
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}"
  LIBRARY DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}"
  ARCHIVE DESTINATION "${OpenIGTLinkIO_ARCHIVE_INSTALL}"
  )

INSTALL(FILES ${${KIT}_HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${KIT}Export.h" DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}")
