#----------------------------------------------------------------------------

if(BUILD_SIFIDETRES_SUPPORT)
    set(SDR_CC "datasources/SDRSource.cc")
    set(SDR_H "datasources/SDRSource.h")
    set(IMPORTER_SIFIDETRES_CC "importers/SSiFiCCDetResImporter.cc")
    set(IMPORTER_SIFIDETRES_H "importers/SSiFiCCDetResImporter.h")
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
        core/SEventBuilderConnector.cc

        database/SDatabase.cc
        database/SRun.cc
        database/SContainer.cc
        database/SCalContainer.cc
        database/SParContainer.cc
        database/SPar.cc
        database/SParSource.cc
        database/SParAsciiSource.cc
        database/SParDatabaseSource.cc
        database/SParRootSource.cc
        database/SDbAbstractInterface.cc
        database/SRESTInterface.cc

        event/SEvent.cc

        datasources/SDataSource.cc
        datasources/SFileSource.cc
        datasources/SDDSource.cc
        datasources/SKSSource.cc
        datasources/SPMISource.cc
        datasources/SCBSource.cc
        datasources/STPSource.cc
        datasources/SPetirocSource.cc
        datasources/STTreeSource.cc
        datasources/SHLDSource.cc

        ${SDR_CC}
        datasources/SRootSource.cc
        datasources/SUnpacker.cc
        datasources/SDDUnpacker.cc

        ${IMPORTER_SIFIDETRES_CC}

        datastruct/SCategory.cc
        datastruct/SCategoryHeader.cc
        datastruct/SCategoryIndex.cc
        datastruct/SDDSamples.cc
        datastruct/SLocator.cc
        datastruct/SLookup.cc

        util/SLoop.cc
        util/SRootFileHeader.cc

    HEADERS
        core/SiFi.h
        core/SCategoryManager.h
        core/STaskManager.h
        core/STask.h
        core/SDetectorManager.h
        core/SDetector.h
        core/SEventBuilderConnector.h

        database/SDatabase.h
        database/SRun.h
        database/SContainer.h
        database/SCalContainer.h
        database/SParContainer.h
        database/SPar.h
        database/SParSource.h
        database/SParAsciiSource.h
        database/SParDatabaseSource.h
        database/SParRootSource.h
        database/SDbAbstractInterface.h
        database/SRESTInterface.h

        event/SEvent.h

        datasources/SDataSource.h
        datasources/SFileSource.h
        datasources/SDDSource.h
        datasources/SKSSource.h
        datasources/SPMISource.h
        datasources/SCBSource.h
        datasources/STPSource.h
        datasources/SPetirocSource.h
        datasources/STTreeSource.h
        datasources/SHLDSource.h

        ${SDR_H}
        datasources/SRootSource.h
        datasources/SUnpacker.h
        datasources/SDDUnpacker.h

        ${IMPORTER_SIFIDETRES_H}

        datastruct/SCategory.h
        datastruct/SCategoryHeader.h
        datastruct/SCategoryIndex.h
        datastruct/SDDSamples.h
        datastruct/SLocator.h
        datastruct/SLookup.h

        util/SLoop.h
        util/SRootFileHeader.h

        ${dabc_BINARY_DIR}/include/hadaq/api.h
        ${dabc_BINARY_DIR}/include/dabc/Url.h
        ${dabc_BINARY_DIR}/include/dabc/api.h
    PRIVATE_LIBRARIES
        ROOT::Core
        ROOT::Tree
        SiFiTools
        Geant
        cpr::cpr
        tabulate::tabulate
        magic_enum::magic_enum
        spdlog::spdlog
        DabcBase
        DabcHadaq
        DabcMbs
    INCLUDE_DIRS
        core
        database
        datasources
        datastruct
        event
        util
        ../fibers
)

if (BUILD_SIFIDETRES_SUPPORT)
    find_package(CCDetectorResponse REQUIRED)

    target_include_directories(SiFi PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/base/importers>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/fibers>
    )
    target_include_directories(G__${ARG_TARGET}_cc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/base/importers>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/fibers>
    )

    target_link_libraries(SiFi PRIVATE
        CCDR::CCDetRes
    )
endif()
