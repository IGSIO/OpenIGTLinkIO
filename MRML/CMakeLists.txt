project(OpenIGTLinkIFMRML)

#
# See CMake/SlicerMacroBuildModuleLogic.cmake for details
#

SET(module_mrml_name "${PROJECT_NAME}")

# The header '${PROJECT_NAME}Export.h' will be automatically configured.
SET(module_mrml_export_directive "Q_SLICER_QTMODULES_OPENIGTLINKIF_MRML_EXPORT")

# Additional directories to include
SET(module_mrml_include_directories
  ${qt_module_include_directories}
  )

# Source files
SET(module_mrml_SRCS
  vtkIGTLToMRMLBase.cxx
  vtkIGTLToMRMLBase.h
  vtkIGTLCircularBuffer.cxx
  vtkIGTLCircularBuffer.h
  vtkMRMLIGTLConnectorNode.cxx
  vtkMRMLIGTLConnectorNode.h
  vtkMRMLIGTLQueryNode.cxx
  vtkMRMLIGTLQueryNode.h
  vtkMRMLIGTLTrackingDataBundleNode.cxx
  vtkMRMLIGTLTrackingDataBundleNode.h
  vtkMRMLIGTLTrackingDataQueryNode.cxx
  vtkMRMLIGTLTrackingDataQueryNode.h
  vtkMRMLImageMetaListNode.cxx
  vtkMRMLImageMetaListNode.h
  )

# Additional Target libraries
SET(module_mrml_target_libraries
  ${ITK_LIBRARIES}
  ${OpenIGTLink_LIBRARIES}
  )


SlicerMacroBuildModuleLogic(
  NAME ${module_mrml_name}
  EXPORT_DIRECTIVE ${module_mrml_export_directive}
  INCLUDE_DIRECTORIES ${module_mrml_include_directories}
  SRCS ${module_mrml_SRCS}
  TARGET_LIBRARIES ${module_mrml_target_libraries}
  )


