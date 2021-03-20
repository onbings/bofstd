# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "openpgm" for configuration "Release"
set_property(TARGET openpgm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(openpgm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/$<TARGET_FILE_NAME:openpgm>"
  )

list(APPEND _IMPORT_CHECK_TARGETS openpgm )
list(APPEND _IMPORT_CHECK_FILES_FOR_openpgm "$<TARGET_FILE_NAME:openpgm>")

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
