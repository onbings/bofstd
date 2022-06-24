#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------
include(Helpers NO_POLICY_SCOPE)
GetTargetOS             (TARGET_OS)

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "openpgm" for configuration "Debug"
set_property(TARGET openpgm APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
if(${TARGET_OS} STREQUAL "LINUX")
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/lib${CMAKE_DEBUG_POSTFIX}.a)
else()
set(LIBLOCATION  ${_IMPORT_PREFIX}/lib/${CMAKE_DEBUG_POSTFIX}.lib)
endif()
message("imp------------------------1----> " ${_IMPORT_PREFIX})
message("imp------------------------2----> " LINUX)
message("imp------------------------3----> " ${LIBLOCATION})
message("imp------------------------4----> " ${CMAKE_DEBUG_POSTFIX})
set_target_properties(openpgm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${LIBLOCATION}"
  )

list(APPEND _IMPORT_CHECK_TARGETS openpgm )
list(APPEND _IMPORT_CHECK_FILES_FOR_openpgm "${LIBLOCATION}" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
