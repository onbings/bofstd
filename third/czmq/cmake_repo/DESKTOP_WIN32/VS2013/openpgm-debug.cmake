#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "openpgm" for configuration "Debug"
set_property(TARGET openpgm APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(openpgm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/$<TARGET_FILE_NAME:openpgm>"
  )

list(APPEND _IMPORT_CHECK_TARGETS openpgm )
list(APPEND _IMPORT_CHECK_FILES_FOR_openpgm "$<TARGET_FILE_NAME:openpgm>")

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
