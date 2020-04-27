#----------------------------------------------------------------------------

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
        datasources/SRootSource.cc
        datasources/SUnpacker.cc
        datasources/SDDUnpacker.cc

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
        datasources/SDDSource.h
        datasources/SFileSource.h
        datasources/SRootSource.h

        datasources/SUnpacker.h
        datasources/SDDUnpacker.h

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
)
