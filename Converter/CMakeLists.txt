project(OpenIGTLinkIFConverter)


#
# See CMake/SlicerMacroBuildModuleLogic.cmake for details
#


SET(module_converter_name "${PROJECT_NAME}")

# The header '${PROJECT_NAME}Export.h' will be automatically configured.
SET(module_converter_export_directive "Q_SLICER_QTMODULES_OPENIGTLINKIF_CONVERTER_EXPORT")


if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_USE_VERSION_2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 2")
else (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 1")
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)


# Additional directories to include
SET(module_converter_include_directories
  ${OpenIGTLinkIFMRML_SOURCE_DIR}
  ${OpenIGTLinkIFMRML_BINARY_DIR}
  ${qt_module_include_directories}
  ${OpenIGTLink_INCLUDE_DIRS}
  )

# Source files
SET(module_converter_SRCS
  vtkIGTLToMRMLImage.cxx
  vtkIGTLToMRMLImage.h
  vtkIGTLToMRMLImageMetaList.cxx
  vtkIGTLToMRMLImageMetaList.h
  vtkIGTLToMRMLLinearTransform.cxx
  vtkIGTLToMRMLLinearTransform.h
  vtkIGTLToMRMLPosition.cxx
  vtkIGTLToMRMLPosition.h
  vtkIGTLToMRMLTrackingData.cxx
  vtkIGTLToMRMLTrackingData.h
  )

# Additional Target libraries
SET(module_converter_target_libraries
  ${ITK_LIBRARIES}
  ${OpenIGTLink_LIBRARIES}
  OpenIGTLinkIFMRML
  )


SlicerMacroBuildModuleLogic(
  NAME ${module_converter_name}
  EXPORT_DIRECTIVE ${module_converter_export_directive}
  INCLUDE_DIRECTORIES ${module_converter_include_directories}
  SRCS ${module_converter_SRCS}
  TARGET_LIBRARIES ${module_converter_target_libraries}
  )


