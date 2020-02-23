#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(MAPT-Framework-Library CXX)

set(MODULES base geant fibers_stack)

link_directories(
    ${MODULES}
)

add_subdirectory(base)
add_subdirectory(geant)
add_subdirectory(fibers_stack)
add_subdirectory(tools)
