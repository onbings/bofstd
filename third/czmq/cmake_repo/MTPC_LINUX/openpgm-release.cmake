# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "openpgm" for configuration "Release"
set_property(TARGET openpgm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/openpgm.a)
message("imp------------------------rel----> " ${_IMPORT_PREFIX})

set_target_properties(openpgm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${LIBLOCATION}"
  )

list(APPEND _IMPORT_CHECK_TARGETS openpgm )
list(APPEND _IMPORT_CHECK_FILES_FOR_openpgm "${LIBLOCATION}" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
