#----------------------------------------------------------------------------

SIFI_GENERATE_LIBRARY(
    TARGET Geant
    SOURCES
        SGeantFibersRaw.cc
        SGeantTrack.cc
    HEADERS
        SGeantFibersRaw.h
        SGeantTrack.h
    PRIVATE_LIBRARIES
        ROOT::Core
        ROOT::Physics
)
