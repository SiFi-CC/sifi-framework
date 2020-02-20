#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(SiFi-Framework-DST CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

link_directories(${PROJECT_SOURCE_DIR}/../lib)
include_directories(${PROJECT_SOURCE_DIR}/../lib/base)
include_directories(${PROJECT_SOURCE_DIR}/../lib/geant)
include_directories(${PROJECT_SOURCE_DIR}/../lib/fibers_stack)
include_directories(${PROJECT_SOURCE_DIR}/../lib/tools)

add_executable(mapt_simdst simdst.cc)
target_link_libraries(mapt_simdst ${ROOT_LIBRARIES})

target_link_libraries(mapt_simdst SiFi GeantSim FibersStack Tools)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS mapt_simdst DESTINATION ${CMAKE_INSTALL_BINDIR})
