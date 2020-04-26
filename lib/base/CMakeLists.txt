#----------------------------------------------------------------------------
# Setup the project
#cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
#project(SiFi-Framework-Library CXX)

SET (LIBRARY_NAME SiFi)

SET (LIBRARY_HDRS
    SiFi.h
    SCategoryManager.h
    SLocator.h
    STaskManager.h
    STask.h
    SDetectorManager.h
    SDetector.h
    SParManager.h

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
    datastruct/SLookup.h
    datastruct/SPar.h
    datastruct/SParContainer.h

    util/SLoop.h
    util/SRootFileHeader.h
)

SET(LIBRARY_SRCS
    SiFi.cc

    SCategoryManager.cc
    SLocator.cc
    STaskManager.cc
    STask.cc
    SDetectorManager.cc
    SDetector.cc
    SParManager.cc

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
    datastruct/SLookup.cc
    datastruct/SPar.cc
    datastruct/SParContainer.cc

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
