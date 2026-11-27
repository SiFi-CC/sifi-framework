#----------------------------------------------------------------------------

set(SSIMIMPORTER_CC "")
set(SSIMIMPORTER_H "")
if(BUILD_SIFIDETRES_SUPPORT)
    set(SSIMIMPORTER_CC ../base/importers/SSimImporter.cc)
    set(SSIMIMPORTER_H  ../base/importers/SSimImporter.h)
endif()

SIFI_GENERATE_LIBRARY(
    TARGET Fibers
    SOURCES
        SFibersDetector.cc
        SFibersGeomPar.cc
        SFibersUnpacker.cc
        SFibersLookup.cc
        SMultiFibersLookup.cc
        SFibersPMIUnpacker.cc
        SFibersCBUnpacker.cc
        SFibersTPUnpacker.cc
        SFibersTP4to1Unpacker.cc
        SFibersPetirocUnpacker.cc
        SFibersTTreeUnpacker.cc
        SFibersHLDUnpacker.cc
        SFibersDDUnpacker.cc
        SFibersDDUnpackerPar.cc
        SFibersDDCalibratorPar.cc
        SFibersTOFPETCalibratorPar.cc
        SSiPMsTOFPETCalibratorPar.cc
        SFiberToSiPMPar.cc
        SFibersRaw.cc
        SFibersRawCluster.cc
        SSiPMHit.cc
        SEventNumber.cc
        SFibersCalibrator.cc
        SFibersCalibratorPar.cc
        SFibersCal.cc
        SFibersDigitizer.cc
        SFibersDigitizerPar.cc
        SFibersCalSim.cc
        SFibersHitFinder.cc
        SFibersHitFinderPar.cc
        SFibersHit.cc
        SFibersHitSim.cc
        SFibersClusterFinder.cc
        SFibersClusterFinderPar.cc
        SFibersCluster.cc
        #SFibersIdentification.cc
        SFibersRawClusterFinder.cc
        SSiPMCluster.cc
        #        SSiPMLookup.cc
        SSiPMClusterFinder.cc
        SDetectorGeomPar.cc
        ${SSIMIMPORTER_CC}
    HEADERS
        SFibersDetector.h
        SFibersGeomPar.h
        SFibersUnpacker.h
        SFibersLookup.h
        SMultiFibersLookup.h
        SFibersPMIUnpacker.h
        SFibersCBUnpacker.h
        SFibersTPUnpacker.h
        SFibersTP4to1Unpacker.h
        SFibersPetirocUnpacker.h
        SFibersTTreeUnpacker.h
        SFibersHLDUnpacker.h
        SFibersDDUnpacker.h
        SFibersDDUnpackerPar.h
        SFibersDDCalibratorPar.h
        SFibersTOFPETCalibratorPar.h
        SSiPMsTOFPETCalibratorPar.h
        SFiberToSiPMPar.h
        SFibersRaw.h
        SFibersRawCluster.h
        SSiPMHit.h
        SEventNumber.h
        SFibersCalibrator.h
        SFibersCalibratorPar.h
        SFibersCal.h
        SFibersDigitizer.h
        SFibersDigitizerPar.h
        SFibersCalSim.h
        SFibersHitFinder.h
        SFibersHitFinderPar.h
        SFibersHit.h
        SFibersHitSim.h
        SFibersClusterFinder.h
        SFibersClusterFinderPar.h
        SFibersCluster.h
        #SFibersIdentification.h
        SFibersRawClusterFinder.h
        SSiPMCluster.h
        #        SSiPMLookup.h
        SSiPMClusterFinder.h
        SDetectorGeomPar.h
        ${SSIMIMPORTER_H}

    PRIVATE_LIBRARIES
        ROOT::Core
        SiFi
        Geant
        dabc::DabcHadaq
)

if (BUILD_SIFIDETRES_SUPPORT)
    # SiPMRLib is only needed when the SiFiDetRes support is enabled. The keyword
    # signature (PRIVATE) is required because helpers.cmake already links this
    # target with the keyword form; mixing plain and keyword calls is an error.
    target_link_libraries(Fibers PRIVATE SiPMR::SiPMRLib)

    # SSimImporter is compiled as part of Fibers (see lib/base/CMakeLists.txt for
    # why), so consumers of Fibers need the importer and datasource headers on
    # their include path. These are BUILD_INTERFACE-only because the headers are
    # not installed.
    target_include_directories(Fibers PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/base/importers>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/base/datasources>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/fibers>
    )
endif()
