# #----------------------------------------------------------------------------

SIFI_GENERATE_LIBRARY(
    TARGET SiFiTools
    SOURCES
        SProgressBar.cc
    HEADERS
        SProgressBar.h
    LIBRARIES
        ROOT::Core
)
