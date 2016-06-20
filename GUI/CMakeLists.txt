project(igtlioGUI)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINKIO_GUI_EXPORT")

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
    
find_package(VTK 6.1
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

find_package(CTK REQUIRED)
include(${CTK_USE_FILE})

set(EXECUTABLE ${PROJECT_NAME})

if( IGTLIO_QT_VERSION VERSION_GREATER "4" )
    find_package( Qt5 COMPONENTS Widgets Concurrent Xml OpenGL Sql REQUIRED )
else()
    find_package( Qt4 )
    include(${QT_USE_FILE})
endif()

set(${KIT}_SRCS
  qIGTLIOGuiUtilities.cxx
  qIGTLIOGuiUtilities.h
  qIGTLIOLogicController.cxx
  qIGTLIOLogicController.h
  qIGTLIOClientWidget.cxx
  qIGTLIOClientWidget.h
  qIGTLIOConnectorListWidget.cxx
  qIGTLIOConnectorListWidget.h
  qIGTLIOConnectorModel.cxx
  qIGTLIOConnectorModel.h
  qIGTLIOConnectorPropertyWidget.cxx
  qIGTLIOConnectorPropertyWidget.h
  qIGTLIODevicesModel.h
  qIGTLIODevicesModel.cxx
  qIGTLIODevicesWidget.h
  qIGTLIODevicesWidget.cxx
  vtkIGTLIONode.h
  vtkIGTLIONode.cxx
  qIGTLIODeviceButtonsWidget.h
  qIGTLIODeviceButtonsWidget.cxx
  qIGTLIODeviceAddWidget.h
  qIGTLIODeviceAddWidget.cxx
  qIGTLIODevicePropertiesWidget.h
  qIGTLIODevicePropertiesWidget.cxx
  DeviceWidgets/qIGTLIODeviceWidgetFactory.h
  DeviceWidgets/qIGTLIODeviceWidgetFactory.cxx
  DeviceWidgets/qIGTLIODeviceWidget.h
  DeviceWidgets/qIGTLIODeviceWidget.cxx
  DeviceWidgets/qIGTLIOGenericDeviceWidget.h
  DeviceWidgets/qIGTLIOGenericDeviceWidget.cxx
  DeviceWidgets/qIGTLIOStatusDeviceWidget.h
  DeviceWidgets/qIGTLIOStatusDeviceWidget.cxx
  DeviceWidgets/qIGTLIOCommandDeviceWidget.h
  DeviceWidgets/qIGTLIOCommandDeviceWidget.cxx
  )

set(${KIT}_MOC_SRCS
  qIGTLIOClientWidget.h
  qIGTLIOLogicController.h
  qIGTLIOConnectorModel.h
  qIGTLIOConnectorListWidget.h
  qIGTLIOConnectorPropertyWidget.h
  qIGTLIODevicesModel.h
  qIGTLIODevicesWidget.h
  qIGTLIODeviceButtonsWidget.h
  qIGTLIODeviceAddWidget.h
  qIGTLIODevicePropertiesWidget.h
  DeviceWidgets/qIGTLIODeviceWidgetFactory.h
  DeviceWidgets/qIGTLIODeviceWidget.h
  DeviceWidgets/qIGTLIOGenericDeviceWidget.h
  DeviceWidgets/qIGTLIOStatusDeviceWidget.h
  DeviceWidgets/qIGTLIOCommandDeviceWidget.h
  )
  
set(${KIT}_UI_SRCS
  ../Resources/UI/qIGTLIOConnectorPropertyWidget.ui
)


set(${KIT}_INCLUDE_DIRECTORIES
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/DeviceWidgets
  ${CMAKE_CURRENT_BINARY_DIR}
  )

if(IGTLIO_QT_VERSION VERSION_GREATER "4")
    qt5_wrap_ui(${KIT}_SRCS ${${KIT}_UI_SRCS})
    qt5_wrap_cpp( ${KIT}_MOC_SRCS ${${KIT}_MOC_SRCS} )
    set( igtlio_qt_libraries Qt5::Widgets )
else()
    QT4_WRAP_CPP( ${KIT}_MOC_SRCS ${${KIT}_MOC_SRCS} )
    QT4_WRAP_UI( ${KIT}_SRCS ${${KIT}_UI_SRCS} )
    set( igtlio_qt_libraries Qt4::QtGui )
endif()

set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  ${CTK_LIBRARIES}
  ${igtlio_qt_libraries}
  igtlioLogic
  )

add_library(${KIT} ${${KIT}_SRCS} ${${KIT}_MOC_SRCS})
target_link_libraries(${KIT} PUBLIC ${${KIT}_TARGET_LIBRARIES})
target_include_directories(${KIT} ${${KIT}_INCLUDE_DIRECTORIES})

include( GenerateExportHeader )
generate_export_header( ${KIT} EXPORT_FILE_NAME ${KIT}Export.h EXPORT_MACRO_NAME ${${KIT}_EXPORT_DIRECTIVE} )


