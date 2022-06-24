#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------
include(Helpers NO_POLICY_SCOPE)
GetTargetOS             (TARGET_OS)

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zeromq" for configuration "Debug"
set_property(TARGET zeromq APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
if(${TARGET_OS} STREQUAL "LINUX")
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/libzeromq${CMAKE_DEBUG_POSTFIX}.a)
else()
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/zeromq${CMAKE_DEBUG_POSTFIX}.lib)
endif()

set_target_properties(zeromq PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${LIBLOCATION}"
  )

list(APPEND _IMPORT_CHECK_TARGETS zeromq )
list(APPEND _IMPORT_CHECK_FILES_FOR_zeromq "${LIBLOCATION}" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
