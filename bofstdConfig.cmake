#Use Helpers to determine supported compilers
include(Helpers NO_POLICY_SCOPE)
include(${CMAKE_CURRENT_LIST_DIR}/dependencies.cmake)

set (THE_PROJECT_NAME bofstd) 
#For Visual Studio get the specific version configuration subdirectory
GetInstallCompilerSubdir(COMPILER_SUBDIR)
#string(TOUPPER ${THE_PROJECT_NAME} PROJECT_NAME_UPPER)
#include the generated configuration according to the compiler version for visual studio
include(${CMAKE_CURRENT_LIST_DIR}${COMPILER_SUBDIR}/${THE_PROJECT_NAME}.cmake)

#Set the fullpath for include files
get_filename_component("${THE_PROJECT_NAME}_INCLUDE_DIRS" "${CMAKE_CURRENT_LIST_DIR}/../include" ABSOLUTE)

#Set the fullpath for libraries
get_filename_component("${THE_PROJECT_NAME}_LIBRARIES_DIRS" "${CMAKE_CURRENT_LIST_DIR}/../lib${COMPILER_SUBDIR}" ABSOLUTE)

#Set the library names imported
SET("${THE_PROJECT_NAME}_LIBRARIES" ${THE_PROJECT_NAME})
 
#Set that the library has been found
set("${THE_PROJECT_NAME}_FOUND" TRUE)
