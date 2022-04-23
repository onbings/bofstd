# ==========================
# == PRIVATE DEPENDENCIES ==
# ==========================

if(BOFSTD_BUILD_TESTS)

  if(NOT TARGET GTest::GTest)
    find_package(GTest REQUIRED)
  endif()

endif()
