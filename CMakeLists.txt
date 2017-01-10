cmake_minimum_required(VERSION 2.8.12.2)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW) # CMake 3.0.0
endif()


###########################################################
project(OpenIGTLinkIO)
###########################################################

set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared or static libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Choose version of Qt
set( IGTLIO_QT_VERSION "5" CACHE STRING "Expected Qt version")
mark_as_advanced( IGTLIO_QT_VERSION )
set_property(CACHE IGTLIO_QT_VERSION PROPERTY STRINGS 4 5)
if(NOT (IGTLIO_QT_VERSION VERSION_EQUAL "4" OR IGTLIO_QT_VERSION VERSION_EQUAL "5"))
    message(FATAL_ERROR "Expected value for IGTLIO_QT_VERSION is either '4' or '5'")
endif()

set (OpenIGTLinkIO_TARGETS igtlioLogic igtlioDevices igtlioConverter ${OpenIGTLinkIO_TARGETS})
set (OpenIGTLinkIO_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    Logic
    Converter
    Devices
    ${OpenIGTLinkIO_INCLUDE_DIRS}
)

add_subdirectory(Converter)
add_subdirectory(Devices)
add_subdirectory(Logic)
add_subdirectory(Tools)

option(IGTLIO_USE_GUI "Build IGTLIO with an user interface" ON)
if (${IGTLIO_USE_GUI})
  add_subdirectory(GUI)
  set (OpenIGTLinkIO_TARGETS igtlioGUI ${OpenIGTLinkIO_TARGETS})
endif()

option(IGTLIO_USE_EXAMPLES "Build IGTLIO examples" ON)
if (${IGTLIO_USE_EXAMPLES})
  add_subdirectory(Examples)
endif ()

# TODO use the namespace feature for all libs
export(TARGETS ${OpenIGTLinkIO_TARGETS}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/OpenIGTLinkIOTargets.cmake"
  )

set (OpenIGTLinkIO_LIBRARIES ${OpenIGTLinkIO_TARGETS})

set (OpenIGTLinkIO_LIBRARY_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(OpenIGTLinkIOConfig.cmake.in
  OpenIGTLinkIOConfig.cmake
  )

include(CTest)

if (BUILD_TESTING)
  add_subdirectory(Testing)
endif()
