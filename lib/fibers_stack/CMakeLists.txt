#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(SiFi-Framework-Library CXX)

SET (LIBRARY_NAME FibersStack)

SET (LIBRARY_HDRS
    SFibersStackDetector.h
    SFibersStackGeomPar.h
    SFibersStackUnpacker.h
    SFibersStackLookup.h
    SFibersStackDDUnpacker.h
    SFibersStackDDUnpackerPar.h
    SFibersStackRaw.h
    SFibersStackCalibrator.h
    SFibersStackCalibratorPar.h
    SFibersStackCal.h
    SFibersStackDigitizer.h
    SFibersStackDigitizerPar.h
    SFibersStackCalSim.h
)

SET(LIBRARY_SRCS
    SFibersStackDetector.cc
    SFibersStackGeomPar.cc
    SFibersStackUnpacker.cc
    SFibersStackLookup.cc
    SFibersStackDDUnpacker.cc
    SFibersStackDDUnpackerPar.cc
    SFibersStackRaw.cc
    SFibersStackCalibrator.cc
    SFibersStackCalibratorPar.cc
    SFibersStackCal.cc
    SFibersStackDigitizer.cc
    SFibersStackDigitizerPar.cc
    SFibersStackCalSim.cc
)

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
install(TARGETS ${LIBRRAY_NAME}
	EXPORT ${CMAKE_PROJECT_NAME}Targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	COMPONENT shlib COMPONENT dev)
