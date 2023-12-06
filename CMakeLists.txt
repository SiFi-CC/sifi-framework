#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(SiFi
    VERSION 0.3.0
    LANGUAGES CXX
)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/Modules
    ${CMAKE_SOURCE_DIR}
    $ENV{ROOTSYS}
)

include(GNUInstallDirs)

# options
set(BUILTIN_CPR AUTO CACHE STRING "Use built-in cpr")
set_property(CACHE BUILTIN_CPR PROPERTY STRINGS AUTO ON OFF)

set(BUILTIN_TABULATE AUTO CACHE STRING "Use built-in tabulate")
set_property(CACHE BUILTIN_TABULATE PROPERTY STRINGS AUTO ON OFF)

set(BUILTIN_MAGICENUM AUTO CACHE STRING "Use built-in magic_enum")
set_property(CACHE BUILTIN_MAGICENUM PROPERTY STRINGS AUTO ON OFF)

# =================================================================================================
# fetch external content
include(FetchContent) # requires >= cmake-3.11

#-------------------------------------------------------------------------------
# cmake-scripts
FetchContent_Declare(cmake-scripts
    GIT_REPOSITORY https://github.com/StableCoder/cmake-scripts.git
    GIT_TAG        774a07ece3945b4f72e3ddc11646ab7cc9a349e2 # from main after sanitizer updates
)

FetchContent_MakeAvailable(cmake-scripts)
list(APPEND CMAKE_MODULE_PATH
    ${cmake-scripts_SOURCE_DIR}
)
include(c++-standards)
include(code-coverage)
include(sanitizers)
include(doxygen)
include(formatting)
include(tools)

#-------------------------------------------------------------------------------
# cpr
SET (REQUIRED_CPR_VERSION 1.6.2)

FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/whoshuu/cpr
    GIT_TAG        ${REQUIRED_CPR_VERSION}
)

if(BUILTIN_CPR STREQUAL "AUTO")
    find_package(cpr ${REQUIRED_CPR_VERSION} QUIET)
    if (NOT cpr_FOUND)
        SET(USE_BUILTIN_CPR TRUE)
    endif()
elseif(BUILTIN_CPR)     # a true value (such as ON) was used
    SET(USE_BUILTIN_CPR TRUE)
else()                  # a false value (such as OFF) was used
    find_package(cpr ${REQUIRED_CPR_VERSION} REQUIRED)
endif()

if (USE_BUILTIN_CPR)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared CPR")
    set(CPR_FORCE_USE_SYSTEM_CURL ON)
    set(CPR_USE_SYSTEM_GTEST ON)
    # based on https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent
    #FetchContent_MakeAvailable(cpr EXCLUDE_FROM_ALL)
    FetchContent_GetProperties(cpr)
    if(NOT cpr_POPULATED)
        FetchContent_Populate(cpr)
        add_subdirectory(${cpr_SOURCE_DIR} ${cpr_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "Uses system-provided cpr")
endif()

#-------------------------------------------------------------------------------
# tabulate
SET (REQUIRED_TABULATE_VERSION 1.4)

FetchContent_Declare(tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate
    GIT_TAG        v${REQUIRED_TABULATE_VERSION}
)

if(BUILTIN_TABULATE STREQUAL "AUTO")
    find_package(tabulate ${REQUIRED_TABULATE_VERSION} QUIET)
    if (NOT tabulate_FOUND)
        SET(USE_BUILTIN_TABULATE TRUE)
    endif()
elseif(BUILTIN_TABULATE)     # a true value (such as ON) was used
    SET(USE_BUILTIN_TABULATE TRUE)
else()                  # a false value (such as OFF) was used
    find_package(tabulate ${REQUIRED_TABULATE_VERSION} REQUIRED)
endif()

if (USE_BUILTIN_TABULATE)
    #FetchContent_MakeAvailable(tabulate)
    FetchContent_GetProperties(tabulate)
    if(NOT tabulate_POPULATED)
        FetchContent_Populate(tabulate)
        add_subdirectory(${tabulate_SOURCE_DIR} ${tabulate_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "Uses system-provided tabulate")
endif()


# magic-enum
SET (REQUIRED_MAGICENUM_VERSION 0.7.3)

FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum
    GIT_TAG        v${REQUIRED_MAGICENUM_VERSION}
)

if(BUILTIN_MAGICENUM STREQUAL "AUTO")
    find_package(magic_enum ${REQUIRED_MAGICENUM_VERSION} QUIET)
    if (NOT magic_enum_FOUND)
        SET(USE_BUILTIN_MAGICENUM TRUE)
    endif()
elseif(BUILTIN_MAGICENUM)     # a true value (such as ON) was used
    SET(USE_BUILTIN_MAGICENUM TRUE)
else()                  # a false value (such as OFF) was used
    find_package(magic_enum ${REQUIRED_MAGICENUM_VERSION} REQUIRED)
endif()

if (USE_BUILTIN_MAGICENUM)
    #FetchContent_MakeAvailable(magic_enum)
    FetchContent_GetProperties(magic_enum)
    if(NOT magic_enum_POPULATED)
        FetchContent_Populate(magic_enum)
        add_subdirectory(${magic_enum_SOURCE_DIR} ${magic_enum_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "Uses system-provided magic_enum")
endif()

#-------------------------------------------------------------------------------
# dabc
set(DABC_LEGACY_INSTALL ON)
FetchContent_Declare(DABC
    #GIT_REPOSITORY https://github.com/linev/dabc
    #GIT_TAG        14ceba3aae0e0a6611f1fb71a12fa81a78c35261
    GIT_REPOSITORY https://github.com/gsi-ee/dabc
    GIT_TAG        master
)
FetchContent_MakeAvailable(DABC)

# =================================================================================================


#-------------------------------------------------------------------------------
##### Symbols export control
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${PROJECT_NAME})

##### set paths
set(_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# use, i.e. don't skip the full RPATH for the build tree
# SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
# SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# the RPATH to be used when installing
#SET(CMAKE_INSTALL_RPATH ${_RPATH})
SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR})

# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

set(CMAKE_CXX_FLAGS -std=c++17)
set(CMAKE_CXX_STANDARD 17)
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



clang_format(format ${ALL_SOURCES})
include_what_you_use("-Xiwyu;any;-Xiwyu;iwyu;-Xiwyu;args")

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

if (CODE_COVERAGE)
    add_code_coverage()
    add_code_coverage_all_targets()
endif()

# Root
find_package(ROOT 6.20 CONFIG REQUIRED COMPONENTS Core Tree)
if (NOT ${ROOT_FOUND})
    message(FATAL_ERROR "ROOT not found.")
endif()
include(${ROOT_USE_FILE})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)



#find_package(ROOT::RHTTP)
#target_link_libraries(ROOT::RHTTP)


#-------------------------------------------------------------------------------
##### HELPER file
include(helpers)


option(BUILD_SIFIDETRES_SUPPORT "Build SiFi Detector Response" ON)
if (BUILD_SIFIDETRES_SUPPORT)
    #find_package(CCDetectorResponse)
    find_package(SiPMResponse)
endif()


#----------------------------------------------------------------------------
include_directories(${CMAKE_BINARY_DIR} ${dabc_BINARY_DIR}/include)

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

uninstall_target()

include(GenerateExportHeader)
generate_export_header(SiFi)

# keep all targets here
set(PROJECT_TARGETS SiFi Fibers Geant SiFiTools)

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
export(TARGETS ${PROJECT_TARGETS} cpr DabcBase DabcHadaq DabcMbs
    FILE ${CMAKE_PROJECT_NAME}Targets.cmake
    NAMESPACE SIFI::)

# Register package in user's package registry
export(PACKAGE SIFI)

option(ENABLE_TESTING "Build tests" OFF)

if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    COMPONENT Development
)

message("")
message("Add to your profile")
message("export SIFIDIR=${CMAKE_INSTALL_PREFIX}")
message("export PATH=${SIFIDIR}/bin:\$\{PATH\}")
message("")
