#Use Helpers to determine supported compilers
include(Helpers NO_POLICY_SCOPE)
 
#For Visual Studio get the specific version configuration subdirectory
GetHostCompiler(HOST_COMPILER)
if(${HOST_COMPILER} STREQUAL "MSVC")
  GetHostCompilerSymbol(HOST_COMPILER_SYMBOL)
  set(COMPILER_SUBDIR "/${HOST_COMPILER_SYMBOL}")
else()
  set(COMPILER_SUBDIR "")
endif()

include(${CMAKE_CURRENT_LIST_DIR}${COMPILER_SUBDIR}/zeromq.cmake)
 
#Set the fullpath for libraries and include files
get_filename_component(OPENPGM_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include" ABSOLUTE)
get_filename_component(OPENPGM_LIBRARIES_DIRS "${CMAKE_CURRENT_LIST_DIR}/../lib${COMPILER_SUBDIR}" ABSOLUTE)
 
#Set the library names imported
SET(OPENPGM_LIBRARIES
  zeromq
)
 
#Set that the library has been found
set(zeromq_FOUND TRUE)
