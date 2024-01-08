#----------------------------------------------------------------------------
# Setup the project

#add_executable(sifi_simdst simdst.cc)
#target_link_libraries(sifi_simdst SiFi Fibers Geant SiFiTools ROOT::Tree)

add_executable(sifi_dst dst.cc)
target_link_libraries(sifi_dst SiFi Fibers Geant SiFiTools ROOT::Tree)

add_executable(sifi_dst_4to1 dst_4to1.cc)
target_link_libraries(sifi_dst_4to1 SiFi Fibers Geant SiFiTools ROOT::Tree)

#add_executable(sifi_dst_4to1_sim dst_4to1_sim.cc)
#target_link_libraries(sifi_dst_4to1_sim SiFi Fibers Geant SiFiTools ROOT::Tree)

install(TARGETS #sifi_simdst 
                sifi_dst 
                sifi_dst_4to1
        DESTINATION ${CMAKE_INSTALL_BINDIR})


#if(BUILD_SIFIDETRES_SUPPORT)
    #add_executable(sifi_detres_importer dst_sifidetres.cc)
    #target_link_libraries(sifi_detres_importer SiFi Fibers Geant SiFiTools ROOT::Tree CCDR::CCDetRes)
    #install(TARGETS sifi_detres_importer DESTINATION ${CMAKE_INSTALL_BINDIR})
#endif()

add_library(SiPMRLib INTERFACE)

target_include_directories(SiPMRLib 
    INTERFACE ${CMAKE_INSTALL_PREFIX}/include
)
target_link_directories(SiPMRLib
    INTERFACE ${CMAKE_INSTALL_PREFIX}/lib
              ${CMAKE_INSTALL_PREFIX}/lib64
)

if(BUILD_SIFIDETRES_SUPPORT)
    add_executable(sifi_dst_4to1_sim dst_4to1_sim.cc)
    target_link_libraries(sifi_dst_4to1_sim SiFi Fibers Geant SiFiTools ROOT::Tree SiPMR::SiPMRLib)
    install(TARGETS sifi_dst_4to1_sim DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
