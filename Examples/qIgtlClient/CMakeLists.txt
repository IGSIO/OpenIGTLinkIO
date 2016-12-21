project(igtlioQtClient)

# =========================================================
#  
# =========================================================

set(EXECUTABLE ${PROJECT_NAME})

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

find_package(VTK
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

if( IGTLIO_QT_VERSION VERSION_GREATER "4" )
    find_package( Qt5 COMPONENTS Widgets REQUIRED )
    set( igtlclient_qt_libraries Qt5::Widgets )
else()
    find_package( Qt4 )
    include(${QT_USE_FILE})
    set( igtlclient_qt_libraries Qt4::QtGui )
endif()

set( SOURCE_FILES main.cpp )

add_executable( ${EXECUTABLE} ${SOURCE_FILES} )

target_link_libraries(${EXECUTABLE}
    PRIVATE
    ${igtlclient_qt_libraries}
    igtlioGUI )
