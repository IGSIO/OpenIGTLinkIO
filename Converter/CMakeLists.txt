project(igtlSupportModuleConverter)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "OPENIGTLINK_SUPPORT_MODULE_CONVERTER_EXPORT")


find_package(VTK 6.1
  COMPONENTS
  vtkIOImage
  vtkImagingMath
  REQUIRED
  NO_MODULE)
include(${VTK_USE_FILE})

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  igtlImageConverter.cxx
  )

set(${KIT}_TARGET_LIBRARIES
  ${OpenIGTLink_LIBRARIES}
  ${VTK_LIBRARIES}
  )

add_library(${KIT} ${${KIT}_SRCS})
target_link_libraries(${KIT} ${KIT}_TARGET_LIBRARIES)
target_include_directories(${KIT} ${KIT}_INCLUDE_DIRECTORIES);
#TODO: Handle EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}

