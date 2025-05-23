if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
   find_program(DEBBUILD_FOUND debuild)
   if (NOT DEBBUILD_FOUND)
      message(STATUS "debuild not found")
   else()

      list(APPEND CPACK_GENERATOR           "DEB")
      set(CPACK_DEBIAN_PACKAGE_RELEASE      "1")
      set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
      set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "onbings")
      set(CPACK_DEBIAN_PACKAGE_SECTION      "devel")
      set(CPACK_DEBIAN_PACKAGE_DEPENDS      "")

      set(deb_releasePkgName   "${CPACK_PACKAGE_NAME}.deb")
      set(deb_debugPkgName     "${CPACK_PACKAGE_NAME}-debug.deb")
      set(deb_develPkgName     "${CPACK_PACKAGE_NAME}-devel.deb")

      set(CPACK_DEB_COMPONENT_INSTALL       ON)
      set(CPACK_DEBIAN_RUNTIME_FILE_NAME    ${deb_releasePkgName})
      set(CPACK_DEBIAN_DEVEL_FILE_NAME      ${deb_develPkgName})
      set(CPACK_DEBIAN_DEBUG_FILE_NAME      ${deb_debugPkgName})
         
      set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
      set(CPACK_DEBIAN_DEVEL_PACKAGE_NAME   ${CPACK_PACKAGE_NAME}-devel)
      set(CPACK_DEBIAN_DEBUG_PACKAGE_NAME   ${CPACK_PACKAGE_NAME}-debug)

   endif()
else()
   message(STATUS "Bad cpack type (deb) for this os (${CMAKE_SYSTEM_NAME})")
endif()
