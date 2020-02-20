#----------------------------------------------------------------------------
# Setup the project
#cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
#project(SiFi-Framework-Library CXX)

SET (LIBRARY_NAME SiFi)

SET (LIBRARY_HDRS
    SiFiManager.h
    SCategory.h
    SCategoryIndex.h
    SCategoryHeader.h
    SLocator.h
    STaskManager.h
    STask.h
    SDetectorManager.h
    SDetector.h
    SParManager.h
    SParContainer.h
    SPar.h
    SLookupManager.h
    SLookupContainer.h
    SLookup.h
    datasources/SUnpacker.h
)

SET(LIBRARY_SRCS
    SiFiManager.cc
    SCategory.cc
    SCategoryIndex.cc
    SCategoryHeader.cc
    SLocator.cc
    STaskManager.cc
    STask.cc
    SDetectorManager.cc
    SDetector.cc
    SParManager.cc
    SParContainer.cc
    SPar.cc
    SLookupManager.cc
    SLookupContainer.cc
    SLookup.cc
    datasources/SUnpacker.cc
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
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
