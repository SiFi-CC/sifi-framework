# #----------------------------------------------------------------------------

SIFI_GENERATE_LIBRARY(
    TARGET SiFiTools
    SOURCES
        SProgressBar.cc
        SHelperFunctions.cc
    HEADERS
        SProgressBar.h
        SHelperFunctions.h
    PRIVATE_LIBRARIES
        ROOT::Core
)
