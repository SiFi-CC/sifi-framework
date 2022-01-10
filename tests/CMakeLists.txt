find_package(GTest)

configure_file(tests_config.h.in tests_config.h)

#file(GLOB tests_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests_*.cc")
set(tests_SRCS
    tests_cpr.cc
    tests_rapidjson.cc
    tests_database.cc
    tests_scontainer.cc
    tests_sparcontainer.cc
    tests_database_ascii_source.cc
    tests_database_mysql_interface.cc
    tests_database_mysql_source.cc
    tests_database_root_source.cc
    tests_database_run_container.cc
)

add_executable(
    gtests
    ${tests_SRCS}
)

if (TARGET GTest::gtest_main)
    # new cmake provided
    set(GTEST_TRG GTest::gtest_main)
else()
    # old cmake and pc files
    set(GTEST_TRG gtest gtest_main)
endif()

target_link_libraries(
    gtests
    PRIVATE
        SiFi SiFiTools Fibers Geant
        ROOT::Core
        ROOT::RIO #ROOT::Physics
        ${GTEST_TRG}
        cpr
)
target_code_coverage(gtests ALL)

gtest_discover_tests(gtests)
