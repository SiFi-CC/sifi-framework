#----------------------------------------------------------------------------
# Setup the project
cmake_policy(PUSH)
cmake_policy(SET CMP0048 NEW)
cmake_policy(POP)
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(SiFi
    VERSION 0.0.1
    LANGUAGES CXX
)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/Modules
    $ENV{ROOTSYS}
    ${CMAKE_SOURCE_DIR}/cmake-scripts
)

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

file(GLOB_RECURSE FFILES *.[hc] *.[hc]pp)
clang_format(format ${FFILES})
include_what_you_use(-Xiwyu)

build_docs(TARGET_NAME doc DOXYFILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in PROCESS_DOXYFILE)

cxx_17()
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
    lib/base/datasources/
    lib/base/datastruct/
    lib/base/event/
    lib/base/util/
    lib/geant/
    lib/fibers_stack/
    lib/tools/
)
# Link directories for libgeometry.so
link_directories(${PROJECT_SOURCE_DIR}/geometry/build)
include_directories(${PROJECT_SOURCE_DIR}/geometry/include)

#----------------------------------------------------------------------------
add_subdirectory(lib)
# add_subdirectory(geometry)
# add_subdirectory(simulation)
# add_subdirectory(event_display)
add_subdirectory(analysis)
add_subdirectory(dst)
#add_subdirectory(particle_filter)

option(BUILD_PYTHON_WRAPPER "Build python wrapper" OFF)
if(BUILD_PYTHON_WRAPPER)
add_subdirectory(python)
endif()

enable_testing()
add_subdirectory(tests)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ${CMAKE_PROJECT_NAME})
string(TOUPPER "${PROJECT_NAME}" CAPS_PROJECT_NAME)

include(GNUInstallDirs)

##### set paths
if(WIN32 AND NOT CYGWIN)
	set(DEF_CMAKE_INSTALL_CMAKEDIR cmake)
else()
	set(DEF_CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
endif()
set(CMAKE_INSTALL_CMAKEDIR ${DEF_CMAKE_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")

include(CMakePackageConfigHelpers)
configure_package_config_file(Config.cmake.in ${CMAKE_PROJECT_NAME}Config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR)

write_basic_package_version_file(${CMAKE_PROJECT_NAME}ConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

install(FILES 
	${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_CMAKEDIR})

# Add all targets to the build-tree export set
export(TARGETS SiFi FibersStack GeantSim
	FILE ${CMAKE_PROJECT_NAME}Targets.cmake)

# # Install the export set for use with the install-tree
# install(TARGETS SiFi Fibers GeantSim
# 	EXPORT ${CMAKE_PROJECT_NAME}Targets
# 	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# 	COMPONENT shlib COMPONENT dev)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
	DESTINATION ${CMAKE_INSTALL_CMAKEDIR} COMPONENT dev)

message("")
message("Add to your profile")
message("export PATH=${CMAKE_INSTALL_PREFIX}/bin:\$\{PATH\}")
message("export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:\$\{LD_LIBRARY_PATH\}")
message("")
