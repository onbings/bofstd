# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zeromq" for configuration "Release"
set_property(TARGET zeromq APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
if(${TARGET_OS} STREQUAL "LINUX")
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/libzeromq.a)
else()
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/zeromq.lib)
endif()

set_target_properties(zeromq PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${LIBLOCATION}"
  )

list(APPEND _IMPORT_CHECK_TARGETS zeromq )
list(APPEND _IMPORT_CHECK_FILES_FOR_zeromq "${LIBLOCATION}" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
