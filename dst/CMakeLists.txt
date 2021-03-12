#----------------------------------------------------------------------------
# Setup the project

add_executable(sifi_simdst simdst.cc)
target_link_libraries(sifi_simdst SiFi Fibers Geant SiFiTools ROOT::Tree)

add_executable(sifi_dst dst.cc)
target_link_libraries(sifi_dst SiFi Fibers Geant SiFiTools ROOT::Tree)

install(TARGETS sifi_simdst sifi_dst DESTINATION ${CMAKE_INSTALL_BINDIR})


if(BUILD_SIFIDETRES_SUPPORT)
    add_executable(sifi_detres_importer dst_sifidetres.cc)
    target_link_libraries(sifi_detres_importer SiFi Fibers Geant SiFiTools ROOT::Tree CCDR::CCDetRes)
    install(TARGETS sifi_detres_importer DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
