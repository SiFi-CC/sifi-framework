#----------------------------------------------------------------------------
# Setup the project

add_executable(sifi_simdst simdst.cc)
target_link_libraries(sifi_simdst SiFi FibersStack Geant Tools)

add_executable(sifi_dst dst.cc)
target_link_libraries(sifi_dst SiFi FibersStack Geant Tools)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS sifi_simdst sifi_dst DESTINATION ${CMAKE_INSTALL_BINDIR})
