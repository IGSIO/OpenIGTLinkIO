project(igtlioGUI)

set(${PROJECT_NAME}_EXPORT_DIRECTIVE "OPENIGTLINKIO_GUI_EXPORT")
    
set(VTK_MODULES
  ${IGTLIO_MODULE_PREFIX}IOImage
  ${IGTLIO_MODULE_PREFIX}ImagingMath
  ${IGTLIO_MODULE_PREFIX}GUISupportQt
)

find_package(VTK REQUIRED NO_MODULE
  COMPONENTS
    ${VTK_MODULES}
  )

if(VTK_VERSION VERSION_LESS 8.9.0)
    include(${VTK_USE_FILE})
endif()

set(OpenIGTLinkIO_Qt_Modules 
  Widgets 
  Concurrent 
  Xml 
  OpenGL 
  Sql
  )

if(IGTLIO_QT_VERSION VERSION_EQUAL "5")
  find_package(Qt5 COMPONENTS ${OpenIGTLinkIO_Qt_Modules} REQUIRED)
  if(Qt5_FOUND AND NOT Qt5_DIR)
    # Sometimes Qt5_DIR is not populated
    get_filename_component(Qt5_DIR ${Qt5Core_DIR} DIRECTORY)
    set(Qt5_DIR ${Qt5_DIR}/Qt5)
  endif()

  # Prepare config file code for Qt
  set(OpenIGTLinkIO_Qt_CONFIG_CODE [=[
    if(Qt5_FOUND)
      # using Qt5
      set(IGTL_Qt5_FOUND True)
    elseif(NOT Qt5_DIR)
      # only set, if not in cache
      set(Qt5_DIR "@Qt5_DIR@")
    endif()
    find_package(Qt5 COMPONENTS @OpenIGTLinkIO_Qt_Modules@ REQUIRED)
    if(IGTL_Qt5_FOUND)
      @_VERSION_CONFIG_CODE@
    endif()
    ]=])

  set(_VERSION_CONFIG_CODE)
  foreach(qt_module IN LISTS OpenIGTLinkIO_Qt_Modules)
    set(_VERSION_CONFIG_CODE "${_VERSION_CONFIG_CODE}\nif(TARGET Qt5::${qt_module} AND NOT \"${Qt5${qt_module}_VERSION}\" VERSION_EQUAL \${Qt5${qt_module}_VERSION})")
    set(_VERSION_CONFIG_CODE "${_VERSION_CONFIG_CODE}\nmessage(SEND_ERROR \"Using different Qt Versions for Qt5::${qt_module} in OpenIGTLinkIO (${Qt5${qt_module}_VERSION}) and superceeding project (\${Qt5${qt_module}_VERSION}).\")")
    set(_VERSION_CONFIG_CODE "${_VERSION_CONFIG_CODE}\nendif()")
  endforeach()

  string(REPLACE "@_VERSION_CONFIG_CODE@" "${_VERSION_CONFIG_CODE}" 
    OpenIGTLinkIO_Qt_CONFIG_CODE ${OpenIGTLinkIO_Qt_CONFIG_CODE}
   )
  string(REPLACE "@Qt5_DIR@" "${Qt5_DIR}" 
    OpenIGTLinkIO_Qt_CONFIG_CODE ${OpenIGTLinkIO_Qt_CONFIG_CODE}
   )
else()
  find_package(Qt6 COMPONENTS ${OpenIGTLinkIO_Qt_Modules} REQUIRED)
  if(Qt6_FOUND AND NOT Qt6_DIR)
    # Sometimes Qt5_DIR is not populated
    get_filename_component(Qt6_DIR ${Qt6Core_DIR} DIRECTORY)
    set(Qt6_DIR ${Qt6_DIR}/Qt6)
  endif()

  # Prepare config file code for Qt
  set(OpenIGTLinkIO_Qt_CONFIG_CODE [=[
    if(Qt6_FOUND)
      # using Qt5
      set(IGTL_Qt6_FOUND True)
    elseif(NOT Qt6_DIR)
      # only set, if not in cache
      set(Qt6_DIR "@Qt6_DIR@")
    endif()
    find_package(Qt6 COMPONENTS @OpenIGTLinkIO_Qt_Modules@ REQUIRED)
    if(IGTL_Qt6_FOUND)
      @_VERSION_CONFIG_CODE@
    endif()
    ]=])

  set(_VERSION_CONFIG_CODE)
  foreach(qt_module IN LISTS OpenIGTLinkIO_Qt_Modules)
    set(_VERSION_CONFIG_CODE "${_VERSION_CONFIG_CODE}\nif(TARGET Qt6::${qt_module} AND NOT \"${Qt6${qt_module}_VERSION}\" VERSION_EQUAL \${Qt6${qt_module}_VERSION})")
    set(_VERSION_CONFIG_CODE "${_VERSION_CONFIG_CODE}\nmessage(SEND_ERROR \"Using different Qt Versions for Qt6::${qt_module} in OpenIGTLinkIO (${Qt6${qt_module}_VERSION}) and superceeding project (\${Qt6${qt_module}_VERSION}).\")")
    set(_VERSION_CONFIG_CODE "${_VERSION_CONFIG_CODE}\nendif()")
  endforeach()

  string(REPLACE "@_VERSION_CONFIG_CODE@" "${_VERSION_CONFIG_CODE}" 
    OpenIGTLinkIO_Qt_CONFIG_CODE ${OpenIGTLinkIO_Qt_CONFIG_CODE}
   )
  string(REPLACE "@Qt6_DIR@" "${Qt6_DIR}"
    OpenIGTLinkIO_Qt_CONFIG_CODE ${OpenIGTLinkIO_Qt_CONFIG_CODE}
   )
endif()

string(REPLACE ";" " " OpenIGTLinkIO_Qt_Modules_Glued "${OpenIGTLinkIO_Qt_Modules}")
string(REPLACE "@OpenIGTLinkIO_Qt_Modules@" "${OpenIGTLinkIO_Qt_Modules_Glued}" OpenIGTLinkIO_Qt_CONFIG_CODE ${OpenIGTLinkIO_Qt_CONFIG_CODE})

# propagate OpenIGTLinkIO_Qt_CONFIG_CODE to Main CMakeLists file
SET(OpenIGTLinkIO_Qt_CONFIG_CODE "${OpenIGTLinkIO_Qt_CONFIG_CODE}" PARENT_SCOPE)

set(${PROJECT_NAME}_SRCS
  qIGTLIOGuiUtilities.cxx
  qIGTLIOLogicController.cxx
  qIGTLIOClientWidget.cxx
  qIGTLIOConnectorListWidget.cxx
  qIGTLIOConnectorModel.cxx
  qIGTLIOConnectorPropertyWidget.cxx
  qIGTLIODevicesModel.cxx
  qIGTLIODevicesWidget.cxx
  vtkIGTLIONode.cxx
  qIGTLIODeviceButtonsWidget.cxx
  qIGTLIODeviceAddWidget.cxx
  qIGTLIODevicePropertiesWidget.cxx
  qIGTLIOCommandWidget.cxx
  DeviceWidgets/qIGTLIODeviceWidgetFactory.cxx
  DeviceWidgets/qIGTLIODeviceWidget.cxx
  DeviceWidgets/qIGTLIOGenericDeviceWidget.cxx
  DeviceWidgets/qIGTLIOStatusDeviceWidget.cxx
  DeviceWidgets/qIGTLIOStringDeviceWidget.cxx
  )

set(${PROJECT_NAME}_MOC_SRCS
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
  qIGTLIOCommandWidget.h
  DeviceWidgets/qIGTLIODeviceWidget.h
  DeviceWidgets/qIGTLIOStatusDeviceWidget.h
  DeviceWidgets/qIGTLIOStringDeviceWidget.h
  )
  
set(${PROJECT_NAME}_HDRS
  ${${PROJECT_NAME}_MOC_SRCS}
  qIGTLIOGuiUtilities.h
  qIGTLIOVtkConnectionMacro.h
  vtkIGTLIONode.h
  DeviceWidgets/qIGTLIODeviceWidgetFactory.h
  DeviceWidgets/qIGTLIODeviceWidgetFactory.h
  DeviceWidgets/qIGTLIOGenericDeviceWidget.h
  )

set(${PROJECT_NAME}_UI_SRCS
  ../Resources/UI/qIGTLIOConnectorPropertyWidget.ui
)

set(${PROJECT_NAME}_INCLUDE_DIRECTORIES PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DeviceWidgets>
  $<INSTALL_INTERFACE:${OpenIGTLinkIO_INCLUDE_INSTALL}>
  $<INSTALL_INTERFACE:${OpenIGTLinkIO_INCLUDE_INSTALL}/DeviceWidgets>
  )

if(IGTLIO_QT_VERSION VERSION_EQUAL "5")
  set( igtlio_qt_libraries 
    Qt5::Widgets 
    Qt5::Concurrent 
    Qt5::Sql 
    Qt5::Xml 
    Qt5::OpenGL
   )
  qt5_wrap_ui(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_UI_SRCS})
  qt5_wrap_cpp( ${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_MOC_SRCS})
else()
  set( igtlio_qt_libraries 
    Qt6::Widgets 
    Qt6::Concurrent 
    Qt6::Sql 
    Qt6::Xml 
    Qt6::OpenGL
   )
  qt6_wrap_ui(${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_UI_SRCS})
  qt6_wrap_cpp( ${PROJECT_NAME}_SRCS ${${PROJECT_NAME}_MOC_SRCS})
endif()

set(${PROJECT_NAME}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  ${igtlio_qt_libraries}
  igtlioLogic
  )

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_TARGET_LIBRARIES})
target_include_directories(${PROJECT_NAME} ${${PROJECT_NAME}_INCLUDE_DIRECTORIES})

generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${PROJECT_NAME}Export.h EXPORT_MACRO_NAME ${${PROJECT_NAME}_EXPORT_DIRECTIVE})

INSTALL(TARGETS ${PROJECT_NAME} EXPORT OpenIGTLinkIO 
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}" COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${OpenIGTLinkIO_ARCHIVE_INSTALL}" COMPONENT Development
  )

# Install DeviceWidget file to subfolder
foreach(file IN LISTS ${PROJECT_NAME}_HDRS)
  IF(${file} MATCHES "^DeviceWidgets")
    install(FILES ${file} DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}/DeviceWidgets" COMPONENT Includes)
  ELSE()
    install(FILES ${file} DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}" COMPONENT Includes)
  ENDIF()
endforeach()

install(FILES 
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Export.h" DESTINATION "${OpenIGTLinkIO_INCLUDE_INSTALL}" COMPONENT Includes
  )
