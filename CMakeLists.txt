# Module name (usually identique to the directory name)
set(qt_module_name "OpenIGTLinkIF")
set(qt_module_title "OpenIGTLink I/F")

# --------------------------------------------------------------------------
# Check if version 2 protocol is available
# Features that depnd on OpenIGTLink Protocol 2 are only available
# if the OpenIGTLink library is compiled with OpenIGTLink_PROTOCOL_VERSION_2
# option.

#add_definitions(-DOpenIGTLinkIF_USE_VERSION_2)
#add_definitions(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 2")

# Add subdirectories
add_subdirectory(MRML)
add_subdirectory(Logic)

set(qt_module_export_directive "Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT")

# Additional includes - Current_{source,binary} and Slicer_{Libs,Base} already included
set(qt_module_include_directories
  ${CMAKE_CURRENT_SOURCE_DIR}/GUI
  ${CMAKE_CURRENT_BINARY_DIR}/GUI
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/MRML
  ${CMAKE_CURRENT_BINARY_DIR}/MRML
  )

# Source files
set(qt_module_SRCS
  qSlicerOpenIGTLinkIFModule.cxx
  qSlicerOpenIGTLinkIFModule.h
  qSlicerOpenIGTLinkIFModuleWidget.cxx
  qSlicerOpenIGTLinkIFModuleWidget.h
  GUI/qMRMLSceneIGTLConnectorModel.cxx
  GUI/qMRMLSceneIGTLConnectorModel.h
  GUI/qMRMLIGTLConnectorTreeView.cxx
  GUI/qMRMLIGTLConnectorTreeView.h
  GUI/qMRMLIGTLIOModel.cxx
  GUI/qMRMLIGTLIOModel.h
  GUI/qMRMLIGTLIOModel_p.h
  GUI/qMRMLIGTLIOTreeView.cxx
  GUI/qMRMLIGTLIOTreeView.h
  GUI/qSlicerIGTLConnectorPropertyWidget.cxx
  GUI/qSlicerIGTLConnectorPropertyWidget.h
  GUI/qSlicerIGTLIONodeSelectorWidget.cxx
  GUI/qSlicerIGTLIONodeSelectorWidget.h
  )

# Headers that should run through moc
set(qt_module_MOC_SRCS
  qSlicerOpenIGTLinkIFModule.h
  qSlicerOpenIGTLinkIFModuleWidget.h
  GUI/qMRMLSceneIGTLConnectorModel.h
  GUI/qMRMLIGTLConnectorTreeView.h
  GUI/qMRMLIGTLIOModel.h
  GUI/qMRMLIGTLIOModel_p.h
  GUI/qMRMLIGTLIOTreeView.h
  GUI/qSlicerIGTLConnectorPropertyWidget.h
  GUI/qSlicerIGTLIONodeSelectorWidget.h
  )

# UI files
set(qt_module_UI_SRCS
  Resources/UI/qSlicerIGTLConnectorPropertyWidget.ui
  Resources/UI/qSlicerIGTLIONodeSelectorWidget.ui
  Resources/UI/qSlicerOpenIGTLinkIFModule.ui
)

# Additional Target libraries
set(qt_module_target_libraries
  vtkSlicerOpenIGTLinkIFModuleLogic
  vtkSlicerOpenIGTLinkIFModuleMRML
  )

# Resources
set(qt_module_resources
  Resources/qSlicerOpenIGTLinkIFModule.qrc
  )

slicerMacroBuildQtModule(
  NAME ${qt_module_name}
  EXPORT_DIRECTIVE ${qt_module_export_directive}
  INCLUDE_DIRECTORIES ${qt_module_include_directories}
  SRCS ${qt_module_SRCS}
  MOC_SRCS ${qt_module_MOC_SRCS}
  UI_SRCS ${qt_module_UI_SRCS}
  TARGET_LIBRARIES ${qt_module_target_libraries}
  RESOURCES ${qt_module_resources}
  )


if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif()
