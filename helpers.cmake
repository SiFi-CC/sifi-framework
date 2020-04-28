
function(SIFI_GENERATE_LIBRARY)
    cmake_parse_arguments(ARG "" "TARGET" "SOURCES;HEADERS;LIBRARIES;" ${ARGN})

ROOT_GENERATE_DICTIONARY(G__${ARG_TARGET}_cc
    ${ARG_HEADERS}
    LINKDEF Linkdef.h
)

# ROOT_GENERATE_ROOTMAP(${ARG_TARGET} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/Linkdef.h)

add_library(${ARG_TARGET} SHARED
    ${ARG_SOURCES}
    G__${ARG_TARGET}_cc
)

target_link_libraries(${ARG_TARGET}
    ${ROOT_LIBRARIES}
    ${ARG_LIBRARIES}
)

set_target_properties(${ARG_TARGET}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER
            "${ARG_HEADERS}"
)

# Install the export set for use with the install-tree
install(TARGETS ${ARG_TARGET}
    EXPORT ${CMAKE_PROJECT_NAME}Targets
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Runtime
        NAMELINK_COMPONENT Development
    PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}${LOCAL_SIFI_TREE}
        COMPONENT Development
)

endfunction(SIFI_GENERATE_LIBRARY)


#-------------------------------------------------------------------------------
##### Adds uninstall target
##### Based on: https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#can-i-do-make-uninstall-with-cmake

macro(uninstall_target)
file(WRITE ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake.in [=[
if(NOT EXISTS "@CMAKE_BINARY_DIR@/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: @CMAKE_BINARY_DIR@/install_manifest.txt")
endif()

file(READ "@CMAKE_BINARY_DIR@/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "@CMAKE_COMMAND@" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif()
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif()
endforeach()
]=])

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()


endmacro(uninstall_target)
