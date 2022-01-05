FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum
    GIT_TAG        v${REQUIRED_MAGICENUM_VERSION}
)

if(SIFI_BUILTIN_MAGICENUM STREQUAL "AUTO")
    find_package(magic_enum ${REQUIRED_MAGICENUM_VERSION} QUIET)
    if (NOT magic_enum_FOUND)
        SET(USE_BUILTIN_MAGICENUM TRUE)
    endif()
elseif(SIFI_BUILTIN_MAGICENUM)     # a true value (such as ON) was used
    SET(USE_BUILTIN_MAGICENUM TRUE)
else()                  # a false value (such as OFF) was used
    find_package(magic_enum ${REQUIRED_MAGICENUM_VERSION} REQUIRED)
endif()

if (USE_BUILTIN_MAGICENUM)
    #FetchContent_MakeAvailable(magic_enum)
    FetchContent_GetProperties(magic_enum)
    if(NOT magic_enum_POPULATED)
        FetchContent_Populate(magic_enum)
        add_subdirectory(${magic_enum_SOURCE_DIR} ${magic_enum_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
else()
    message(STATUS "Uses system-provided magic_enum")
endif()
