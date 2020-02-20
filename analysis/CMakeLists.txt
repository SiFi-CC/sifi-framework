# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
#project(SiFi-Framework-Analysis CXX)

link_directories(${CMAKE_BINARY_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/lib/base)
include_directories(${PROJECT_SOURCE_DIR}/lib/geant)
include_directories(${PROJECT_SOURCE_DIR}/lib/fibers_stack)
include_directories(${PROJECT_SOURCE_DIR}/lib/tools)

add_executable(mapt_analysis analysis.cc)
target_link_libraries(mapt_analysis
    ${ROOT_LIBRARIES}
    SiFi GeantSim FibersStack Tools)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS mapt_analysis DESTINATION ${CMAKE_INSTALL_BINDIR})
