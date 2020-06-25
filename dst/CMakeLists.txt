#----------------------------------------------------------------------------
# Setup the project

add_executable(sifi_simdst simdst.cc)
target_link_libraries(sifi_simdst SiFi FibersStack Geant SiFiTools)

add_executable(sifi_dst dst.cc)
target_link_libraries(sifi_dst SiFi FibersStack Geant SiFiTools)

if(BUILD_SIFIDETRES_SUPPORT AND HAVE_SIFIDETRES_SUPPORT)
    add_executable(sifi_detres_importer dst_sifidetres.cc)
    target_link_libraries(sifi_detres_importer SiFi FibersStack Geant SiFiTools)
endif()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS sifi_simdst sifi_dst DESTINATION ${CMAKE_INSTALL_BINDIR})
