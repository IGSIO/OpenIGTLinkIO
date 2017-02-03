project(igtlioQtClient)

# =========================================================
#  
# =========================================================

find_package( VTK
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  vtkGUISupportQt
  REQUIRED
  NO_MODULE
  )
include(${VTK_USE_FILE})

if( IGTLIO_QT_VERSION VERSION_GREATER "4")
  find_package( Qt5 COMPONENTS Widgets REQUIRED)
  set( igtlclient_qt_libraries Qt5::Widgets)
else()
  find_package( Qt4)
  include(${QT_USE_FILE})
  set( igtlclient_qt_libraries Qt4::QtGui)
endif()

set( igtlioQtClient_SRCS 
  main.cpp 
  )

add_executable( ${PROJECT_NAME} ${igtlioQtClient_SRCS})

target_link_libraries( ${PROJECT_NAME}
  PRIVATE
    ${igtlclient_qt_libraries}
    igtlioGUI 
  )

INSTALL(TARGETS ${PROJECT_NAME} EXPORT OpenIGTLinkIO 
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}"
  LIBRARY ARCHIVE DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}"
  )