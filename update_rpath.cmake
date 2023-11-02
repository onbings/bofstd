# This function updates the RPath of the given File  update_rpath(File: The path to the File RPath: The RPath to update
# Check RPATH value "readelf -d <path_to_app> | grep RUNPATH"  => (RUNPATH)            Library runpath: [../lib]
function(update_rpath File RPath)
  find_program(PATCHELF patchelf)
  message(STATUS "Looking for 'patchelf' ${PATCHELF}")
  if (NOT PATCHELF STREQUAL "PATCHELF-NOTFOUND")
    if(NOT IS_SYMLINK ${File}) # Only process realfiles
      message(STATUS "Updating RPath of '${File}' to '${RPath}'")
      execute_process(COMMAND ${PATCHELF} --set-rpath  "${RPath}" ${File} RESULT_VARIABLE result)
      if(NOT ${result} EQUAL 0)
        message(STATUS "Failed to update RPath of '${File}' to '${RPath}' (error ${result})")
      endif()
    endif()
  endif()
endfunction()

# Call the function, passing the arguments we defined in add_custom_command.
update_rpath(${File} ${RPath})