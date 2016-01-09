project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_OPENIGTLINKIF_MODULE_MRML_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${Slicer_Base_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Logic
  ${CMAKE_CURRENT_BINARY_DIR}/../Logic
  )

set(${KIT}_SRCS
  vtkIGTLCircularBuffer.cxx
  vtkIGTLToMRMLBase.cxx
  vtkIGTLToMRMLImage.cxx
  vtkIGTLToMRMLLinearTransform.cxx
  vtkIGTLToMRMLPosition.cxx
  vtkIGTLToMRMLStatus.cxx
  vtkMRMLIGTLConnectorNode.cxx
  vtkMRMLIGTLStatusNode.cxx
  )

if(OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  list(APPEND ${KIT}_SRCS
    vtkMRMLIGTLQueryNode.cxx
    vtkMRMLImageMetaListNode.cxx
    vtkMRMLLabelMetaListNode.cxx
    vtkMRMLTextNode.cxx
    vtkIGTLToMRMLPoints.cxx
    vtkIGTLToMRMLPolyData.cxx
    vtkIGTLToMRMLImageMetaList.cxx
    vtkIGTLToMRMLLabelMetaList.cxx
    vtkIGTLToMRMLTrackingData.cxx
    vtkIGTLToMRMLSensor.cxx
    vtkIGTLToMRMLString.cxx
    vtkIGTLToMRMLTrajectory.cxx
    vtkMRMLIGTLTrackingDataQueryNode.cxx
    vtkMRMLIGTLTrackingDataBundleNode.cxx
    vtkMRMLIGTLSensorNode.cxx
    )
endif()

set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${MRML_LIBRARIES}
  SlicerBaseLogic
  vtkSlicerAnnotationsModuleMRML
  vtkSlicerMarkupsModuleMRML
  igtlSupportModuleCodec
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
