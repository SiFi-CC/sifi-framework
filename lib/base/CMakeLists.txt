#----------------------------------------------------------------------------

message(STATUS "Got it ${HAVE_SIFIDETRES_SUPPORT}")
if(BUILD_SIFIDETRES_SUPPORT AND HAVE_SIFIDETRES_SUPPORT)
    set(SDR_CC "datasources/SDRSource.cc")
    set(SDR_H "datasources/SDRSource.h")

    set(IMPORTER_SIFIDETRES_CC "importers/SSiFiCCDetResImporter.cc")
    set(IMPORTER_SIFIDETRES_H "importers/SSiFiCCDetResImporter.h")
    include_directories(importers)

    find_library(OPLIB OPHitLib HINTS ${CMAKE_INSTALL_PREFIX})
    message(STATUS "OP Library = ${OPLIB} ${OPLIB_FOUND}")
    set(EXTRA_DEPS ${OPLIB})

    find_library(GEOMLIB SiFiGeomLib HINTS ${CMAKE_INSTALL_PREFIX})
    message(STATUS "Struct Library = ${GEOMLIB} ${GEOMLIB_FOUND}")
    list(APPEND EXTRA_DEPS ${GEOMLIB})

    message(STATUS "EXTRA_DEPS: ${EXTRA_DEPS}")
endif()

SIFI_GENERATE_LIBRARY(
    TARGET SiFi
    SOURCES
        core/SiFi.cc
        core/SCategoryManager.cc
        core/STaskManager.cc
        core/STask.cc
        core/SDetectorManager.cc
        core/SDetector.cc
        core/SParManager.cc

        event/SEvent.cc

        datasources/SDataSource.cc
        datasources/SFileSource.cc
        datasources/SDDSource.cc
        ${SDR_CC}
        datasources/SRootSource.cc
        datasources/SUnpacker.cc
        datasources/SDDUnpacker.cc

        ${IMPORTER_SIFIDETRES_CC}

        datastruct/SCalContainer.cc
        datastruct/SCategory.cc
        datastruct/SCategoryHeader.cc
        datastruct/SCategoryIndex.cc
        datastruct/SDDSamples.cc
        datastruct/SLocator.cc
        datastruct/SLookup.cc
        datastruct/SPar.cc
        datastruct/SParContainer.cc

        util/SLoop.cc
        util/SRootFileHeader.cc
    HEADERS
        core/SiFi.h
        core/SCategoryManager.h
        core/STaskManager.h
        core/STask.h
        core/SDetectorManager.h
        core/SDetector.h
        core/SParManager.h

        event/SEvent.h

        datasources/SDataSource.h
        datasources/SFileSource.h
        datasources/SDDSource.h
        ${SDR_H}
        datasources/SRootSource.h
        datasources/SUnpacker.h
        datasources/SDDUnpacker.h

        ${IMPORTER_SIFIDETRES_H}

        datastruct/SCalContainer.h
        datastruct/SCategory.h
        datastruct/SCategoryHeader.h
        datastruct/SCategoryIndex.h
        datastruct/SDDSamples.h
        datastruct/SLocator.h
        datastruct/SLookup.h
        datastruct/SPar.h
        datastruct/SParContainer.h

        util/SLoop.h
        util/SRootFileHeader.h

    LIBRARIES
        ${EXTRA_DEPS} FibersStack Geant
)
