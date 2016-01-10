project(igtlSupportModuleCodec)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINK_SUPPORT_MODULE_CODEC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
#  ${Slicer_Base_INCLUDE_DIRS}
#  ${CMAKE_CURRENT_SOURCE_DIR}/../Logic
#  ${CMAKE_CURRENT_BINARY_DIR}/../Logic
  )

set(${KIT}_SRCS
  igtlCodecImage.cxx
  )

set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${MRML_LIBRARIES}
  SlicerBaseLogic
#  vtkSlicerAnnotationsModuleMRML
#  vtkSlicerMarkupsModuleMRML
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
