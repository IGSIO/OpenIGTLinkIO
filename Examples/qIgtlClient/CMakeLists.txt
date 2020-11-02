project(igtlioQtClient)

# =========================================================
#  Reference Qt client implementation
# =========================================================
find_package(VTK REQUIRED NO_MODULE
  COMPONENTS
    ${IGTLIO_MODULE_PREFIX}IOImage
    ${IGTLIO_MODULE_PREFIX}ImagingMath
    ${IGTLIO_MODULE_PREFIX}GUISupportQt
  )
if(VTK_VERSION VERSION_LESS 8.9.0)
  include(${VTK_USE_FILE})
endif()

set(${PROJECT_NAME}_SRCS 
  main.cpp 
  )

set(${PROJECT_NAME}_TARGET_LIBRARIES
  igtlioGUI
  )

if(IGTLIO_QT_VERSION VERSION_GREATER "4")
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
  list(APPEND ${PROJECT_NAME}_TARGET_LIBRARIES Qt5::Widgets)
else()
  find_package(Qt4)
  include(${QT_USE_FILE})
  list(APPEND ${PROJECT_NAME}_TARGET_LIBRARIES Qt4::QtGui)
endif()

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_TARGET_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} EXPORT OpenIGTLinkIO 
  RUNTIME DESTINATION "${OpenIGTLinkIO_BINARY_INSTALL}" COMPONENT RuntimeLibraries
  LIBRARY DESTINATION "${OpenIGTLinkIO_LIBRARY_INSTALL}" COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION "${OpenIGTLinkIO_ARCHIVE_INSTALL}" COMPONENT Development
  )