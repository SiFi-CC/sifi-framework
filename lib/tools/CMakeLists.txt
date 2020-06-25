# #----------------------------------------------------------------------------

if(BUILD_SIFIDETRES_SUPPORT AND HAVE_SIFIDETRES_SUPPORT)
    set(IMPORTER_SIFIDETRES_CC "importers/SSiFiCCDetResImporter.cc")
    set(IMPORTER_SIFIDETRES_H "importers/SSiFiCCDetResImporter.h")
    include_directories(importers)
endif()

SIFI_GENERATE_LIBRARY(
    TARGET SiFiTools
    SOURCES
        SProgressBar.cc
        ${IMPORTER_SIFIDETRES_CC}
    HEADERS
        SProgressBar.h
        ${IMPORTER_SIFIDETRES_H}
)
