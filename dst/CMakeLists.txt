#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(SiFi-Framework-DST CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

link_directories(${PROJECT_SOURCE_DIR}/../lib)

add_executable(sifi_simdst simdst.cc)
target_link_libraries(sifi_simdst ${ROOT_LIBRARIES})
target_link_libraries(sifi_simdst SiFi GeantSim FibersStack Tools)

add_executable(sifi_dst dst.cc)
target_link_libraries(sifi_dst ${ROOT_LIBRARIES})
target_link_libraries(sifi_dst SiFi GeantSim FibersStack Tools)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS sifi_simdst DESTINATION ${CMAKE_INSTALL_BINDIR})
