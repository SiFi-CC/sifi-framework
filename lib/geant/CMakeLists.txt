#----------------------------------------------------------------------------

SIFI_GENERATE_LIBRARY(
    TARGET Geant
    SOURCES
        SGeantFibersRaw.cc
        SGeantTrack.cc
    HEADERS
        SGeantFibersRaw.h
        SGeantTrack.h
    LIBRARIES
        ROOT::Core
)
