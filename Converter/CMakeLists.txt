project(igtlioConverter)

set(${PROJECT_NAME}_EXPORT_DIRECTIVE "OPENIGTLINKIO_CONVERTER_EXPORT")

set(VTK_MODULES
  vtkIOImage
  vtkImagingMath
)

find_package(VTK REQUIRED NO_MODULE
  COMPONENTS
  ${VTK_MODULES}
  )
include(${VTK_USE_FILE})

set(${PROJECT_NAME}_INCLUDE_DIRECTORIES PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${OpenIGTLinkIO_INCLUDE_INSTALL}>
  )
set(${PROJECT_NAME}_SRCS
  igtlioBaseConverter.cxx
  igtlioCommandConverter.cxx
  igtlioConverterUtilities.cxx
  igtlioImageConverter.cxx
  igtlioImageMetaConverter.cxx
  igtlioLabelMetaConverter.cxx
  igtlioPolyDataConverter.cxx
  igtlioPointConverter.cxx
  igtlioStatusConverter.cxx
  igtlioStringConverter.cxx
  igtlioTransformConverter.cxx
  )

set(${PROJECT_NAME}_HDRS
  igtlioBaseConverter.h
  igtlioCommandConverter.h
  igtlioConverterUtilities.h
  igtlioImageConverter.h
  igtlioImageMetaConverter.h
  igtlioLabelMetaConverter.h
  igtlioPolyDataConverter.h
  igtlioPointConverter.h
  igtlioStatusConverter.h
  igtlioStringConverter.h
  igtlioTransformConverter.h
  igtlioUsSectorDefinitions.h
  )

IF(OpenIGTLink_ENABLE_VIDEOSTREAMING)
  LIST(APPEND ${PROJECT_NAME}_SRCS
  igtlioVideoConverter.cxx
  )
  LIST(APPEND ${PROJECT_NAME}_HDRS
  igtlioVideoConverter.h
  )
ENDIF()

set(${PROJECT_NAME}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  )

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_TARGET_LIBRARIES})
target_include_directories(${PROJECT_NAME} ${${PROJECT_NAME}_INCLUDE_DIRECTORIES})

generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${PROJECT_NAME}Export.h EXPORT_MACRO_NAME ${${PROJECT_NAME}_EXPORT_DIRECTIVE})

# --------------------------------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME} EXPORT OpenIGTLinkIO
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}" COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${OpenIGTLinkIO_ARCHIVE_INSTALL}" COMPONENT Development
  )

INSTALL(FILES ${${PROJECT_NAME}_HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Export.h" DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}" COMPONENT Includes)
