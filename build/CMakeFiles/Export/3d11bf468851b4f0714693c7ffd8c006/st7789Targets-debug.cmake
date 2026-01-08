#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "st7789::st7789" for configuration "Debug"
set_property(TARGET st7789::st7789 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(st7789::st7789 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libst7789.a"
  )

list(APPEND _cmake_import_check_targets st7789::st7789 )
list(APPEND _cmake_import_check_files_for_st7789::st7789 "${_IMPORT_PREFIX}/lib/libst7789.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
