project(vtkSlicerOpenIGTLinkIFModuleLogic)

#
# See CMake/SlicerMacroBuildModuleLogic.cmake for details
#

SET(module_logic_name "${PROJECT_NAME}")

# The header '${PROJECT_NAME}Export.h' will be automatically configured.
SET(module_logic_export_directive "VTK_SLICER_OPENIGTLINKIF_MODULE_LOGIC_EXPORT")

# Additional directories to include
SET(module_logic_include_directories
  ${OpenIGTLinkIFMRML_SOURCE_DIR}
  ${OpenIGTLinkIFMRML_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Converters
  ${CMAKE_CURRENT_BINARY_DIR}/Converters
  )

SET(openigtlinkif_converters_srcs
  Converters/vtkIGTLToMRMLBase.cxx
  Converters/vtkIGTLToMRMLImage.cxx
  Converters/vtkIGTLToMRMLImageMetaList.cxx
  Converters/vtkIGTLToMRMLLinearTransform.cxx
  Converters/vtkIGTLToMRMLPosition.cxx
  Converters/vtkIGTLToMRMLTrackingData.cxx
)

# Source files
SET(module_logic_SRCS
  vtkSlicerOpenIGTLinkIFLogic.cxx
  ${openigtlinkif_converters_src}
  )

# Additional Target libraries
SET(module_logic_target_libraries
  ${ITK_LIBRARIES}
  )


SlicerMacroBuildModuleLogic(
  NAME ${module_logic_name}
  EXPORT_DIRECTIVE ${module_logic_export_directive}
  INCLUDE_DIRECTORIES ${module_logic_include_directories}
  SRCS ${module_logic_SRCS}
  TARGET_LIBRARIES ${module_logic_target_libraries}
  )
