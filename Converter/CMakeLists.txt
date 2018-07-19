project(igtlioConverter)

set(${PROJECT_NAME}_EXPORT_DIRECTIVE "OPENIGTLINKIO_CONVERTER_EXPORT")

set(VTK_MODULES
  vtkIOImage
  vtkImagingMath
)
if(VTK_WRAP_PYTHON)
  list(APPEND VTK_MODULES
    vtkWrappingPythonCore
    )
endif()

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
  igtlioImageConverter.cxx
  igtlioPolyDataConverter.cxx
  igtlioStatusConverter.cxx
  igtlioTransformConverter.cxx
  igtlioCommandConverter.cxx
  igtlioStringConverter.cxx
  igtlioConverterUtilities.cxx
  )

set(${PROJECT_NAME}_HDRS
  igtlioBaseConverter.h
  igtlioImageConverter.h
  igtlioPolyDataConverter.h
  igtlioStatusConverter.h
  igtlioTransformConverter.h
  igtlioStringConverter.h
  igtlioUsSectorDefinitions.h
  igtlioConverterUtilities.h
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
# Python Wrapping
# --------------------------------------------------------------------------
if(VTK_WRAP_PYTHON)
  vtkMacroPythonWrap(
    KIT_NAME ${PROJECT_NAME}
    KIT_SRCS ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS} 
    KIT_INSTALL_BIN_DIR "${OpenIGTLinkIO_BINARY_INSTALL}"
    KIT_INSTALL_LIB_DIR "${OpenIGTLinkIO_LIBRARY_INSTALL}"
    )
endif()
# --------------------------------------------------------------------------

INSTALL(TARGETS ${PROJECT_NAME} EXPORT OpenIGTLinkIO 
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}" COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${OpenIGTLinkIO_ARCHIVE_INSTALL}" COMPONENT Development
  )

INSTALL(FILES ${${PROJECT_NAME}_HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Export.h" DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}" COMPONENT Includes)
