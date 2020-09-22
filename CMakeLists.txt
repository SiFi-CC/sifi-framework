#----------------------------------------------------------------------------
# Setup the project
cmake_policy(PUSH)
cmake_policy(SET CMP0048 NEW)
cmake_policy(POP)
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(SiFi
    VERSION 0.1.0
    LANGUAGES CXX
)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/Modules
    ${CMAKE_SOURCE_DIR}
    $ENV{ROOTSYS}
    ${CMAKE_SOURCE_DIR}/cmake-scripts
)

#-------------------------------------------------------------------------------
##### HELPER file
include(helpers)

#-------------------------------------------------------------------------------
##### Symbols export control
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${CMAKE_PROJECT_NAME})
string(TOUPPER "${PROJECT_NAME}" CAPS_PROJECT_NAME)

option(SIFI_TREE "SiFi Install Tree" OFF)

##### set paths
if(WIN32 AND NOT CYGWIN)
    set(DEF_CMAKE_INSTALL_CMAKEDIR cmake)
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL "8")
        set(DEF_CMAKE_INSTALL_LIBDIR lib64)
    else()
        set(DEF_CMAKE_INSTALL_LIBDIR lib)
    endif()
    set(DEF_CMAKE_INSTALL_CMAKEDIR ${DEF_CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
endif()

if (SIFI_TREE)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/sifi)
    set(LOCAL_SIFI_TREE "")
    set(CMAKE_INSTALL_LIBDIR ${DEF_CMAKE_INSTALL_LIBDIR}${LOCAL_SIFI_TREE} CACHE PATH "Object code libraries" FORCE)
    set(_RPATH "\$ORIGIN/../${DEF_CMAKE_INSTALL_LIBDIR}")
else()
    set(LOCAL_SIFI_TREE "")
    set(CMAKE_INSTALL_LIBDIR ${DEF_CMAKE_INSTALL_LIBDIR}${LOCAL_SIFI_TREE} CACHE PATH "Object code libraries" FORCE)
    set(_RPATH "\$ORIGIN/../${DEF_CMAKE_INSTALL_LIBDIR}${LOCAL_SIFI_TREE}")
endif()
set(CMAKE_INSTALL_CMAKEDIR ${DEF_CMAKE_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files" FORCE)

# use, i.e. don't skip the full RPATH for the build tree
# SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
# SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH ${_RPATH})

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)


set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
# option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" OFF)
# if(WITH_GEANT4_UIVIS)
#   find_package(Geant4 QUIET ui_all vis_all)
# else()
#   find_package(Geant4 QUIET)
# endif()

include(c++-standards)
include(code-coverage)
include(sanitizers)
include(doxygen)
include(formatting)
include(tools)

file(GLOB_RECURSE FFILES S*.[hc] S*.[hc]pp)
clang_format(format ${FFILES})
include_what_you_use(-Xiwyu)

option(ROOT_DOC_XREF "Root Cross-reference in doc" OFF)

if (ROOT_DOC_XREF)
    set(TAGFILES ${CMAKE_BINARY_DIR}/ROOT.tag=https://root.cern/doc/master/)
else()
    unset(TAGFILES)
endif()

build_docs(ADD_TO_DOC DOXYFILE_PATH ${CMAKE_SOURCE_DIR}/Doxyfile.in PROCESS_DOXYFILE)

if (ROOT_XREF)
    add_custom_target(
        ROOT_tag
        COMMAND curl https://root.cern.ch/doc/master/ROOT.tag -o ROOT.tag -C -
        BYPRODUCTS ROOT.tag
    )
    add_dependencies(doc ROOT_tag)
endif()


#cxx_17()
# add_code_coverage()
add_code_coverage_all_targets()

# Root
find_package(ROOT QUIET REQUIRED COMPONENTS Core)
include_directories(${ROOT_INCLUDE_DIR})

if (DEFINED ROOT_USE_FILE)
include(${ROOT_USE_FILE})
endif()

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#----------------------------------------------------------------------------
# Setup include directory for this project
include_directories(
    lib/base/
    lib/base/core/
    lib/base/datasources/
    lib/base/datasources/
    lib/base/datastruct/
    lib/base/event/
    lib/base/importers/
    lib/base/util/
    lib/geant/
    lib/fibers_stack/
    lib/tools/
    lib/tools/importers/
    ${CMAKE_BINARY_DIR}
)
# Link directories for libgeometry.so
# link_directories(${PROJECT_SOURCE_DIR}/geometry/build)
# include_directories(${PROJECT_SOURCE_DIR}/geometry/include)

option(BUILD_SIFIDETRES_SUPPORT "Build SiFi Detector Response" OFF)
if (BUILD_SIFIDETRES_SUPPORT)
    set(SIFIDETRES_PATH "" CACHE PATH "Installation or source directory of SiFi Detector Response")
    find_file(HAVE_SIFIDETRES_SUPPORT
        NAMES DR_Cluster.hh
        PATHS ${SIFIDETRES_PATH}
        PATH_SUFFIXES include
        REQUIRED)
    if (HAVE_SIFIDETRES_SUPPORT)
        get_filename_component(SIFIDETRES_INCLUDE ${HAVE_SIFIDETRES_SUPPORT} DIRECTORY)
        message(STATUS "SiFi Det Res = ${HAVE_SIFIDETRES_SUPPORT}")
        include_directories(AAA ${SIFIDETRES_INCLUDE})
    endif()
endif()

#----------------------------------------------------------------------------
add_subdirectory(lib)
# add_subdirectory(geometry)
# add_subdirectory(simulation)
# add_subdirectory(event_display)
# add_subdirectory(analysis)
add_subdirectory(dst)
#add_subdirectory(particle_filter)

configure_file(SiFiConfig.h.in SiFiConfig.h)

option(BUILD_PYTHON_WRAPPER "Build python wrapper" OFF)
if(BUILD_PYTHON_WRAPPER)
add_subdirectory(python)
endif()

enable_testing()
add_subdirectory(tests)

uninstall_target()

include(GenerateExportHeader)
generate_export_header(SiFi)

# keep all targets here
set(PROJECT_TARGETS SiFi FibersStack Geant SiFiTools)

include(CMakePackageConfigHelpers)
configure_package_config_file(Config.cmake.in ${CMAKE_PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR)

write_basic_package_version_file(${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
        ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)
install(FILES
        ${CMAKE_BINARY_DIR}/sifi_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}${LOCAL_SIFI_TREE}
)

# Add all targets to the build-tree export set
export(TARGETS ${PROJECT_TARGETS}
    FILE ${CMAKE_PROJECT_NAME}Targets.cmake)

# # Install the export set for use with the install-tree
# install(TARGETS SiFi Fibers GeantSim
#     EXPORT ${CMAKE_PROJECT_NAME}Targets
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     COMPONENT shlib COMPONENT dev)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    COMPONENT Development
)

include(GenerateExportHeader)
foreach(TRG ${TARGETS})
    generate_export_header(${TRG})
endforeach()

message("")
message("Add to your profile")
message("export SIFIDIR=${CMAKE_INSTALL_PREFIX}")
message("export PATH=${SIFIDIR}/bin:\$\{PATH\}")
message("")
