set (THE_PROJECT_NAME bofstd)
include(${CMAKE_CURRENT_LIST_DIR}/${THE_PROJECT_NAME}.cmake)

#Set the fullpath for include files
get_filename_component("${THE_PROJECT_NAME}_INCLUDE_DIRS" "${CMAKE_CURRENT_LIST_DIR}/../../../include" ABSOLUTE)

#Set the fullpath for libraries
get_filename_component("${THE_PROJECT_NAME}_LIBRARIES_DIRS" "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

#Set the library names imported
SET("${THE_PROJECT_NAME}_LIBRARIES" ${THE_PROJECT_NAME})
 
#Set that the library has been found
set("${THE_PROJECT_NAME}_FOUND" TRUE)

#message("==CMAKE_CURRENT_LIST_DIR==================>" ${CMAKE_CURRENT_LIST_DIR})
#message("==THE_PROJECT_NAME========================>" ${THE_PROJECT_NAME})
#message("==${THE_PROJECT_NAME}_INCLUDE_DIRS========>" ${${THE_PROJECT_NAME}_INCLUDE_DIRS})
#message("==${THE_PROJECT_NAME}_LIBRARIES_DIRS======>" ${${THE_PROJECT_NAME}_LIBRARIES_DIRS})