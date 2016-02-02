project(igtlioQtClient)

# =========================================================
#  
# =========================================================

set(EXECUTABLE ${PROJECT_NAME})

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

find_package(Qt5Widgets REQUIRED)

set( MOC_HEADER_FILES
)

set( SOURCE_FILES
    main.cpp
)


qt5_wrap_cpp( MOC_HEADER_FILES ${MOC_HEADER_FILES} )
#qt5_add_resources( QT_RESOURCE_FILES resource.qrc )

add_executable(${EXECUTABLE} 
    ${QT_RESOURCE_FILES} 
    ${MOC_HEADER_FILES} 
    ${SOURCE_FILES}
)
target_link_libraries(${EXECUTABLE}
    PRIVATE
    Qt5::Widgets
    igtlioGUI
    )


