project(igtlioTesting)

set(KIT "${PROJECT_NAME}")


find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
    
find_package(VTK
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
  )

set(${KIT}_MOC_SRCS
  )
  
set(${KIT}_UI_SRCS
)


set(${KIT}_INCLUDE_DIRECTORIES
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
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



add_executable(testClientServer testClientServer.cxx ${${KIT}_SRCS} ${${KIT}_MOC_SRCS})
target_link_libraries(testClientServer PUBLIC ${${KIT}_TARGET_LIBRARIES})
target_include_directories(testClientServer ${${KIT}_INCLUDE_DIRECTORIES})


add_test("testClientServer" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testClientServer)

