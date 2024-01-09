include(CMakeFindDependencyMacro)

macro(find_package_dependency)

  # When loading the *Config.cmake we should
  # call find_dependency which is just a wrapper
  # around find_package to display better
  # messages to the user. When directly dealing
  # with our CMakeLists.txt, we should call
  # find_package directly
  if(FROM_CONFIG_FILE)
     find_dependency(${ARGN})
  else()
     find_package(${ARGN})
  endif()

endmacro()

# ===========================
# == OPTIONAL DEPENDENCIES ==
# ===========================

if(BOFSTD_BUILD_TESTS)
####  find_package(GTest REQUIRED)
endif()

# ===========================
# == REQUIRED DEPENDENCIES ==
# ===========================
if(NOT TARGET jsoncpp_object)
  find_package_dependency(jsoncpp REQUIRED)

  if(TARGET jsoncpp_lib)
    add_library(jsoncpp::jsoncpp ALIAS jsoncpp_lib)
  elseif(TARGET jsoncpp_static)
    add_library(jsoncpp::jsoncpp ALIAS jsoncpp_static)
  else()
    message(FATAL_ERROR "Unknown target for jsoncpp")
  endif()
endif()

#just above find_package_dependency(jsoncpp REQUIRED)

####find_package_dependency(spdlog REQUIRED)
find_package_dependency(fmt REQUIRED)
find_package_dependency(date REQUIRED)

