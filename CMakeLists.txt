
# Add subdirectories
ADD_SUBDIRECTORY(MRML)
ADD_SUBDIRECTORY(Logic)


# Module name (usually identique to the directory name)
SET(qt_module_name "OpenIGTLinkIF")

SET(qt_module_export_directive "Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT")

# Additional includes - Current_{source,binary} and Slicer_{Libs,Base} already included
SET(qt_module_include_directories
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/MRML
  ${CMAKE_CURRENT_BINARY_DIR}/MRML
  )

# Source files
SET(qt_module_SRCS
  qSlicerOpenIGTLinkIFModule.cxx
  qSlicerOpenIGTLinkIFModule.h
  qSlicerOpenIGTLinkIFModuleWidget.cxx
  qSlicerOpenIGTLinkIFModuleWidget.h
  ${openigtlinkif_logic_srcs}
  )

# Headers that should run through moc
SET(qt_module_MOC_SRCS
  qSlicerOpenIGTLinkIFModule.h
  qSlicerOpenIGTLinkIFModuleWidget.h
  )

# UI files
SET(qt_module_UI_SRCS
  Resources/UI/qSlicerOpenIGTLinkIFModule.ui
)

# Additional Target libraries
SET(qt_module_target_libraries
  vtkSlicerOpenIGTLinkIFModuleLogic
  OpenIGTLinkIFMRML
  )

# Resources
SET(qt_module_resources
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

target_link_libraries(qSlicer${qt_module_name}Module
  ${OpenIGTLink_LIBRARIES}
)

if(BUILD_TESTING)
  #add_subdirectory(Testing)
endif(BUILD_TESTING)
