# Copyright (c) 2014, Evs. All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
# PURPOSE.
# 
# Name: 		CMakeLists.txt
# Author: 		Bernard HARMEL
# Revision:		1.0
# 
# Remarks: 
# None
# 
# History:		05 June 2014: Initial version

###############################################################################
cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0057 NEW) # if IN_LIST
macro(GetAllLinkedLibraries MainLib AllTheLib)
message("--------> enter with ${MainLib}")
   if(NOT TARGET ${MainLib})
        message(WARNING "${MainLib} is not a target")
    else()
        get_target_property(LibPath ${MainLib} LOCATION)
message("--------> Path=${LibPath} out of ${AllTheLib}")
        if(NOT ${LibPath} IN_LIST ${AllTheLib})
            list(APPEND ${AllTheLib} ${LibPath})
        endif()
        get_target_property(LinkedLibraries ${MainLib} INTERFACE_LINK_LIBRARIES)
message("--------> LinkedLibraries ${LinkedLibraries}")
        if(NOT "${LinkedLibraries}" STREQUAL "")
            FOREACH(LinkedLibrary ${LinkedLibraries})
			       message("--------> ${LinkedLibrary} out of ${LinkedLibraries}")
				if (NOT LinkedLibrary MATCHES ".*-NOTFOUND")
					GetAllLinkedLibraries(${LinkedLibrary} ${AllTheLib})
				endif()
            ENDFOREACH()
        endif()
    endif()
endmacro()
GetAllLinkedLibraries("czmq" AllTheLibLinkedWithCzmq)
message(STATUS ">>>>czmq lib = ${AllTheLibLinkedWithCzmq}")
list(GET AllTheLibLinkedWithCzmq 1 ZeroMqLib)
message(STATUS ">>>>zmq lib = ${ZeroMqLib}")
