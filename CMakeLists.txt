cmake_minimum_required(VERSION 3.4)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW) # CMake 3.0.0
endif()

###########################################################
project(OpenIGTLinkIO)
###########################################################

set(OpenIGTLinkIO_VERSION_SUFFIX "")

set(OpenIGTLinkIO_BINARY_INSTALL "bin")
set(OpenIGTLinkIO_LIBRARY_INSTALL "lib")
set(OpenIGTLinkIO_CMAKE_INSTALL "${OpenIGTLinkIO_LIBRARY_INSTALL}/igtlio/cmake")
set(OpenIGTLinkIO_PACKAGE_INSTALL "${OpenIGTLinkIO_CMAKE_INSTALL}/igtlio${OpenIGTLinkIO_VERSION_SUFFIX}")
set(OpenIGTLinkIO_INCLUDE_INSTALL "include/igtlio${OpenIGTLinkIO_VERSION_SUFFIX}")


find_package(OpenIGTLink REQUIRED NO_MODULE)
include(${OpenIGTLink_USE_FILE})

if(NOT BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared or static libs")
endif()

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

# Choose version of Qt
if(NOT IGTLIO_QT_VERSION)
  set(IGTLIO_QT_VERSION "5" CACHE STRING "Expected Qt version")
  mark_as_advanced( IGTLIO_QT_VERSION )
endif()
set_property(CACHE IGTLIO_QT_VERSION PROPERTY STRINGS 4 5)

if(NOT (IGTLIO_QT_VERSION VERSION_EQUAL "4" OR IGTLIO_QT_VERSION VERSION_EQUAL "5"))
    message(FATAL_ERROR "Expected value for IGTLIO_QT_VERSION is either '4' or '5'")
endif()

set (OpenIGTLinkIO_TARGETS igtlioLogic igtlioDevices igtlioConverter)
set (OpenIGTLinkIO_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Converter
  ${CMAKE_CURRENT_SOURCE_DIR}/Devices
  )

add_subdirectory(Converter)
add_subdirectory(Devices)
add_subdirectory(Logic)

option(IGTLIO_USE_GUI "Build IGTLIO with an user interface" ON)
if (${IGTLIO_USE_GUI})
  add_subdirectory(GUI)
  list (APPEND OpenIGTLinkIO_TARGETS igtlioGUI)
endif()

option(IGTLIO_USE_EXAMPLES "Build IGTLIO examples" ON)
if (${IGTLIO_USE_EXAMPLES})
  add_subdirectory(Examples)
endif ()

# TODO use the namespace feature for all libs
export(TARGETS ${OpenIGTLinkIO_TARGETS}
  FILE "${CMAKE_BINARY_DIR}/OpenIGTLinkIOTargets.cmake"
  )

set (OpenIGTLinkIO_LIBRARIES ${OpenIGTLinkIO_TARGETS})
set (OpenIGTLinkIO_LIBRARY_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

if(IGTLIO_USE_GUI AND IGTLIO_QT_VERSION EQUAL "5" AND NOT Qt5_DIR)
  # Sometimes Qt5_DIR is not populated
  get_filename_component(Qt5_DIR ${Qt5Core_DIR} DIRECTORY)
  set(Qt5_DIR ${Qt5_DIR}/Qt5)
endif()

set(IS_INSTALL False)
configure_file(OpenIGTLinkIOConfig.cmake.in
  OpenIGTLinkIOConfig.cmake
  @ONLY
  )

set(IS_INSTALL True)
configure_file(OpenIGTLinkIOConfig.cmake.in
  install/OpenIGTLinkIOConfig.cmake
  @ONLY
  )

include(CTest)

if (BUILD_TESTING)
  add_subdirectory(Testing)
endif()

install(EXPORT OpenIGTLinkIO
  DESTINATION "${OpenIGTLinkIO_PACKAGE_INSTALL}"
  FILE "OpenIGTLINKIOTargets.cmake"
  )

install(FILES ${CMAKE_BINARY_DIR}/install/OpenIGTLinkIOConfig.cmake
  DESTINATION "${OpenIGTLinkIO_PACKAGE_INSTALL}"
  )