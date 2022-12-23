#
# vcpkg_android.cmake 
#
# Helper script when using vcpkg with cmake. It should be triggered via the variable VCPKG_TARGET_ANDROID
#
# For example:
# if (VCPKG_TARGET_ANDROID)
#     include("cmake/vcpkg_android.cmake")
# endif()
#   
# Note: VCPKG_TARGET_ANDROID is not an official Vcpkg variable. 
# it is introduced for the need of this script

message("==vcpkg_android.cmake==INPUT VAR:")
message("==vcpkg_android.cmake==ANDROID_PLATFORM===============> " ${ANDROID_PLATFORM})
message("==vcpkg_android.cmake==CMAKE_INSTALL_PREFIX===========> " ${CMAKE_INSTALL_PREFIX})
message("==vcpkg_android.cmake==CMAKE_BUILD_TYPE===============> " ${CMAKE_BUILD_TYPE})
message("==vcpkg_android.cmake==CMAKE_TOOLCHAIN_FILE===========> " ${CMAKE_TOOLCHAIN_FILE})
message("==vcpkg_android.cmake==VCPKG_CHAINLOAD_TOOLCHAIN_FILE=> " ${VCPKG_CHAINLOAD_TOOLCHAIN_FILE})
message("==vcpkg_android.cmake==CMAKE_LIBRARY_OUTPUT_DIRECTORY=> " ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
message("==vcpkg_android.cmake==CMAKE_ARCHIVE_OUTPUT_DIRECTORY=> " ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
message("==vcpkg_android.cmake==CMAKE_RUNTIME_OUTPUT_DIRECTORY=> " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message("==vcpkg_android.cmake==CMAKE_DEBUG_POSTFIX============> " ${CMAKE_DEBUG_POSTFIX})

message("==vcpkg_android.cmake==INPUT ENV VAR:")
message("==vcpkg_android.cmake==ANDROID_NDK_HOME=====> " $ENV{ANDROID_NDK_HOME})
message("==vcpkg_android.cmake==VCPKG_ROOT===========> " $ENV{VCPKG_ROOT})
message("==vcpkg_android.cmake==CPP_ANDROID_ROOT=====> " $ENV{CPP_ANDROID_ROOT})
message("==vcpkg_android.cmake==CPP_ANDROID_BLD======> " $ENV{CPP_ANDROID_BLD})
message("==vcpkg_android.cmake==CPP_ANDROID_REPO=====> " $ENV{CPP_ANDROID_REPO})
message("==vcpkg_android.cmake==CPP_ANDROID_ABI======> " $ENV{CPP_ANDROID_ABI})
message("==vcpkg_android.cmake==CPP_ANDROID_API======> " $ENV{CPP_ANDROID_API})

# message("==vcpkg_android.cmake=======> Start of variable")
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
# message("==vcpkg_android.cmake=======> End of variable")

if(NOT "${ANDROID_PLATFORM}")
    #
    # 1. Check the presence of environment variable ANDROID_NDK_HOME
    #
    if (NOT DEFINED ENV{ANDROID_NDK_HOME})
        message(FATAL_ERROR "
        Please set an environment variable ANDROID_NDK_HOME
        For example:
        export ANDROID_NDK_HOME=/home/your-account/Android/Sdk/ndk-bundle
        Or:
        export ANDROID_NDK_HOME=/home/your-account/Android/android-ndk-r21b
        ")
    endif()
    #
    # 2. Check the presence of environment variable VCPKG_ROOT
    #
    if (NOT DEFINED ENV{VCPKG_ROOT})
        message(FATAL_ERROR "
        Please set an environment variable VCPKG_ROOT
        For example:
        export VCPKG_ROOT=/path/to/vcpkg
        ")
    endif()

    #
    # 3. Set VCPKG_TARGET_TRIPLET according to CPP_ANDROID_ABI
    # 
    # There are four different Android ABI, each of which maps to 
    # a vcpkg triplet. The following table outlines the mapping from vcpkg architectures to android architectures
    #
    # |VCPKG_TARGET_TRIPLET       | CPP_ANDROID_ABI          |
    # |---------------------------|----------------------|
    # |arm64-android              | arm64-v8a            |
    # |arm-android                | armeabi-v7a          |
    # |x64-android                | x86_64               |
    # |x86-android                | x86                  |
    #
    # The variable must be stored in the cache in order to successfully the two toolchains. 
    #
    if ($ENV{CPP_ANDROID_ABI} MATCHES "arm64-v8a")
	    string(CONCAT CPP_VCPKG_TARGET_TRIPLET "arm64-android-" $ENV{CPP_ANDROID_API})
        set(VCPKG_TARGET_TRIPLET ${CPP_VCPKG_TARGET_TRIPLET} CACHE STRING "" FORCE)
    elseif($ENV{CPP_ANDROID_ABI} MATCHES "armeabi-v7a")
	    string(CONCAT CPP_VCPKG_TARGET_TRIPLET "arm-android-" $ENV{CPP_ANDROID_API})
        set(VCPKG_TARGET_TRIPLET ${CPP_VCPKG_TARGET_TRIPLET} CACHE STRING "" FORCE)
    elseif($ENV{CPP_ANDROID_ABI} MATCHES "x86_64")
	    string(CONCAT CPP_VCPKG_TARGET_TRIPLET "x64-android-" $ENV{CPP_ANDROID_API})
        set(VCPKG_TARGET_TRIPLET ${CPP_VCPKG_TARGET_TRIPLET} CACHE STRING "" FORCE)
    elseif($ENV{CPP_ANDROID_ABI} MATCHES "x86")
	    string(CONCAT CPP_VCPKG_TARGET_TRIPLET "x86-android-" $ENV{CPP_ANDROID_API})
        set(VCPKG_TARGET_TRIPLET ${CPP_VCPKG_TARGET_TRIPLET} CACHE STRING "" FORCE)
    else()
        message(FATAL_ERROR "
        Please specify CPP_ANDROID_ABI
        For example
        cmake ... -DCPP_ANDROID_ABI=armeabi-v7a

        Possible ABIs are: arm64-v8a, armeabi-v7a, x64-android, x86-android
        ")
    endif()
	
    message("==vcpkg_android.cmake==OUTPUT VAR:")
    message("==vcpkg_android.cmake==VCPKG_TARGET_TRIPLET===========> " ${VCPKG_TARGET_TRIPLET})
    message("==vcpkg_android.cmake==VCPKG_CMAKE_SYSTEM_VERSION=====> " ${VCPKG_CMAKE_SYSTEM_VERSION})
    message("==vcpkg_android.cmake==CMAKE_SYSTEM_VERSION===========> " ${CMAKE_SYSTEM_VERSION})
else()
    message(FATAL_ERROR "Incoherent behavior !")
endif()

