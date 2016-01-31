project(igtlioGUI)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINKIO_GUI_EXPORT")

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
    
find_package(VTK 6.1
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  vtkRenderingQt
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

set(EXECUTABLE ${PROJECT_NAME})
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(${KIT}_SRCS
  qIGTLIOLogicController.cxx
  qIGTLIOLogicController.h
  qIGTLIOClientWidget.cxx
  qIGTLIOClientWidget.h
  qIGTLIOConnectorListWidget.cxx
  qIGTLIOConnectorListWidget.h
#  qSlicerOpenIGTLinkIFModule.cxx
#  qSlicerOpenIGTLinkIFModule.h
#  qSlicerOpenIGTLinkIFModuleWidget.cxx
#  qSlicerOpenIGTLinkIFModuleWidget.h
  qIGTLIOConnectorModel.cxx
  qIGTLIOConnectorModel.h
#  GUI/qMRMLIGTLConnectorTreeView.cxx
#  GUI/qMRMLIGTLConnectorTreeView.h
#  GUI/qMRMLIGTLIOModel.cxx
#  GUI/qMRMLIGTLIOModel.h
#  GUI/qMRMLIGTLIOModel_p.h
#  GUI/qMRMLIGTLIOTreeView.cxx
#  GUI/qMRMLIGTLIOTreeView.h
#  GUI/qSlicerIGTLConnectorPropertyWidget.cxx
#  GUI/qSlicerIGTLConnectorPropertyWidget.h
#  GUI/qSlicerIGTLIONodeSelectorWidget.cxx
#  GUI/qSlicerIGTLIONodeSelectorWidget.h
  )

set(${KIT}_MOC_SRCS
  qIGTLIOClientWidget.h
  qIGTLIOLogicController.h
#  qSlicerOpenIGTLinkIFModule.h
#  qSlicerOpenIGTLinkIFModuleWidget.h
  qIGTLIOConnectorModel.h
  qIGTLIOConnectorListWidget.h
#  GUI/qMRMLIGTLConnectorTreeView.h
#  GUI/qMRMLIGTLIOModel.h
#  GUI/qMRMLIGTLIOTreeView.h
#  GUI/qSlicerIGTLConnectorPropertyWidget.h
#  GUI/qSlicerIGTLIONodeSelectorWidget.h
  )
  
  

set(${KIT}_INCLUDE_DIRECTORIES
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )


set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  Qt5::Widgets
  igtlioLogic
  )

qt5_wrap_cpp( ${KIT}_MOC_SRCS ${${KIT}_MOC_SRCS} )
add_library(${KIT} ${${KIT}_SRCS} ${${KIT}_MOC_SRCS})
target_link_libraries(${KIT} PUBLIC ${${KIT}_TARGET_LIBRARIES})
target_include_directories(${KIT} ${${KIT}_INCLUDE_DIRECTORIES})

include( GenerateExportHeader )
generate_export_header( ${KIT} EXPORT_FILE_NAME ${KIT}Export.h EXPORT_MACRO_NAME ${${KIT}_EXPORT_DIRECTIVE} )


