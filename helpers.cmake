
macro(SIFI_GENERATE_LIBRARY)
    cmake_parse_arguments(ARG "" "TARGET" "SOURCES;HEADERS;PRIVATE_HEADERS;LIBRARIES;PRIVATE_LIBRARIES;INCLUDE_DIRS" ${ARGN})

set(BI_INCLUDE_DIRS "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

# split headers from directories, later add headers directly to rootcling and pass include dirs
foreach(_hdr ${ARG_HEADERS})
    if (IS_ABSOLUTE ${_hdr})
       list(APPEND _headers_files ${_hdr})
    else()
        get_filename_component(_comp_dir ${_hdr} DIRECTORY)
        if ("${_comp_dir}" STREQUAL "")
            set(_comp_dir ".")
        endif()
        list(APPEND _component_includes ${_comp_dir})

        get_filename_component(_comp_name ${_hdr} NAME)
        list(APPEND _headers_files ${_comp_name})
    endif()
endforeach()

list(REMOVE_DUPLICATES _component_includes)

foreach(_hdr ${_component_includes})
    get_filename_component(_comp_dir ${_hdr} ABSOLUTE)
    list(APPEND BI_INCLUDE_DIRS "$<BUILD_INTERFACE:${_comp_dir}>")
endforeach()

foreach(incdir ${ARG_INCLUDE_DIRS})
    get_filename_component(fincdir ${incdir} ABSOLUTE)
    list(APPEND BI_INCLUDE_DIRS "$<BUILD_INTERFACE:${fincdir}>")
endforeach()

list(REMOVE_DUPLICATES BI_INCLUDE_DIRS)


# ROOT_GENERATE_ROOTMAP(${ARG_TARGET} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/Linkdef.h)

add_library(${ARG_TARGET} SHARED ${ARG_SOURCES})
add_library(SiFi::${ARG_TARGET} ALIAS ${ARG_TARGET})

get_property(ALL_SOURCES GLOBAL PROPERTY ALL_SOURCES)
list(APPEND ALL_SOURCES ${ARG_SOURCES} ${ARG_HEADERS} ${ARG_PRIVATE_HEADERS})
set_property(GLOBAL PROPERTY ALL_SOURCES "${ALL_SOURCES}")

target_include_directories(${ARG_TARGET}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        ${BI_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        ${CMAKE_BINARY_DIR}
)

ROOT_GENERATE_DICTIONARY(G__${ARG_TARGET}_cc
    ${_headers_files}
    MODULE ${ARG_TARGET}
    LINKDEF Linkdef.h
)

target_link_libraries(${ARG_TARGET}
    PUBLIC
        ${ARG_LIBRARIES}
    PRIVATE
        ${ARG_PRIVATE_LIBRARIES}
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
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

endmacro(SIFI_GENERATE_LIBRARY)


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
