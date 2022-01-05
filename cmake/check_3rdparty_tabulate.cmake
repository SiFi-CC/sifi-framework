FetchContent_Declare(tabulate
    GIT_REPOSITORY https://github.com/p-ranav/tabulate
    GIT_TAG        v${REQUIRED_TABULATE_VERSION}
)

if(SIFI_BUILTIN_TABULATE STREQUAL "AUTO")
    find_package(tabulate ${REQUIRED_TABULATE_VERSION} QUIET)
    if (NOT tabulate_FOUND)
        SET(USE_BUILTIN_TABULATE TRUE)
    endif()
elseif(SIFI_BUILTIN_TABULATE)     # a true value (such as ON) was used
    SET(USE_BUILTIN_TABULATE TRUE)
else()                  # a false value (such as OFF) was used
    find_package(tabulate ${REQUIRED_TABULATE_VERSION} REQUIRED)
endif()

if (USE_BUILTIN_TABULATE)
    #FetchContent_MakeAvailable(tabulate)
    FetchContent_GetProperties(tabulate)
    if(NOT tabulate_POPULATED)
        FetchContent_Populate(tabulate)
        add_subdirectory(${tabulate_SOURCE_DIR} ${tabulate_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "Uses system-provided tabulate")
endif()
