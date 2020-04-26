#----------------------------------------------------------------------------
# Setup the project
#cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
#project(SiFi-Framework-Library CXX)

SET (LIBRARY_NAME SiFi)

SET (LIBRARY_HDRS
    SiFi.h
    SCategory.h
    SCategoryIndex.h
    SCategoryHeader.h
    SCategoryManager.h
    SLocator.h
    STaskManager.h
    STask.h
    SDetectorManager.h
    SDetector.h
    SParManager.h
    SParContainer.h
    SPar.h
    SCalContainer.h
    SLookup.h

    event/SEvent.h

    datasources/SDataSource.h
    datasources/SFileSource.h
    datasources/SDDSource.h
    datasources/SRootSource.h

    datasources/SUnpacker.h
    datasources/SDDUnpacker.h

    datastruct/SDDSamples.h

    util/SLoop.h
    util/SRootFileHeader.h
)

SET(LIBRARY_SRCS
    SiFi.cc
    SCategory.cc
    SCategoryIndex.cc
    SCategoryHeader.cc
    SCategoryManager.cc
    SLocator.cc
    STaskManager.cc
    STask.cc
    SDetectorManager.cc
    SDetector.cc
    SParManager.cc
    SParContainer.cc
    SPar.cc
    SCalContainer.cc
    SLookup.cc

    event/SEvent.cc

    datasources/SDataSource.cc
    datasources/SFileSource.cc
    datasources/SDDSource.cc
    datasources/SRootSource.cc

    datasources/SUnpacker.cc
    datasources/SDDUnpacker.cc

    datastruct/SDDSamples.cc

    util/SLoop.cc
    util/SRootFileHeader.cc
)

link_directories(${CMAKE_BINARY_DIR})

ROOT_GENERATE_DICTIONARY(G__${LIBRARY_NAME}_cc
    ${LIBRARY_HDRS}
    LINKDEF Linkdef.h
)

# ROOT_GENERATE_ROOTMAP(${LIBRARY_NAME} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/Linkdef.h)

add_library(${LIBRARY_NAME} SHARED
    ${LIBRARY_SRCS}
    G__${LIBRARY_NAME}_cc
)

target_link_libraries(${LIBRARY_NAME} ${ROOT_LIBRARIES})

install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${LIBRARY_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the export set for use with the install-tree
install(TARGETS ${LIBRARY_NAME}
	EXPORT ${CMAKE_PROJECT_NAME}Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT shlib COMPONENT dev)
