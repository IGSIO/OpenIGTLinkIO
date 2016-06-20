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
    
add_subdirectory(Converter)
add_subdirectory(Devices)
add_subdirectory(Logic)
add_subdirectory(GUI) # TODO make optional
add_subdirectory(Examples) # TODO make optional

# TODO use the namespace feature for all libs
export(TARGETS igtlioLogic igtlioDevices igtlioConverter
  FILE "${CMAKE_CURRENT_BINARY_DIR}/OpenIGTLinkIOTargets.cmake"
  )


set (OpenIGTLinIO_LIBRARIES igtlioLogic igtlioDevices igtlioConverter)
set (OpenIGTLinIO_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    Logic
    Converter
    Devices
)
set (OpenIGTLinIO_LIBRARY_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(OpenIGTLinkIOConfig.cmake.in
  OpenIGTLinkIOConfig.cmake
  )


