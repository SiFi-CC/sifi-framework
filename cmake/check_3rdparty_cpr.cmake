FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/whoshuu/cpr
    GIT_TAG        ${REQUIRED_CPR_VERSION}
)

if(SIFI_BUILTIN_CPR STREQUAL "AUTO")
    find_package(cpr ${REQUIRED_CPR_VERSION} QUIET)
    if (NOT cpr_FOUND)
        SET(USE_BUILTIN_CPR TRUE)
    endif()
elseif(SIFI_BUILTIN_CPR)     # a true value (such as ON) was used
    SET(USE_BUILTIN_CPR TRUE)
else()                  # a false value (such as OFF) was used
    find_package(cpr ${REQUIRED_CPR_VERSION} REQUIRED)
endif()

if (USE_BUILTIN_CPR)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared CPR")
    set(CPR_FORCE_USE_SYSTEM_CURL ON)
    set(CPR_USE_SYSTEM_GTEST ON)
    # based on https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent
    #FetchContent_MakeAvailable(cpr EXCLUDE_FROM_ALL)
    FetchContent_GetProperties(cpr)
    if(NOT cpr_POPULATED)
        FetchContent_Populate(cpr)
        add_subdirectory(${cpr_SOURCE_DIR} ${cpr_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
    set_property(TARGET cpr PROPERTY POSITION_INDEPENDENT_CODE ON)
else()
    message(STATUS "Uses system-provided cpr")
endif()
