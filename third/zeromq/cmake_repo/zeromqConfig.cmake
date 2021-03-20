#Use Helpers to determine supported compilers
include(Helpers NO_POLICY_SCOPE)
 
#For Visual Studio get the specific version configuration subdirectory
GetInstallCompilerSubdir(COMPILER_SUBDIR)
string(TOUPPER zeromq PROJECT_NAME_UPPER)
include(${CMAKE_CURRENT_LIST_DIR}${COMPILER_SUBDIR}/zeromq.cmake)
 
#Set the fullpath for libraries and include files
get_filename_component("${PROJECT_NAME_UPPER}_INCLUDE_DIRS" "${CMAKE_CURRENT_LIST_DIR}/../include" ABSOLUTE)
get_filename_component("${PROJECT_NAME_UPPER}_LIBRARIES_DIRS" "${CMAKE_CURRENT_LIST_DIR}/../lib${COMPILER_SUBDIR}" ABSOLUTE)
 
#Set the library names imported
SET(${PROJECT_NAME_UPPER}_LIBRARIES  zeromq)
 
#Set that the library has been found
set(${PROJECT_NAME_UPPER}_FOUND TRUE)
