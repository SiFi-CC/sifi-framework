find_package(GTest)

enable_testing()

configure_file(tests_config.h.in tests_config.h)

file(GLOB tests_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests_*.cc")

add_executable(
    gtests
    ${tests_SRCS}
)

target_link_libraries(
    gtests
    PRIVATE
        SiFi SiFiTools Fibers Geant
        ROOT::Core
        ROOT::RIO #ROOT::Physics
        GTest::gtest_main
)
target_code_coverage(gtests ALL)

gtest_discover_tests(gtests)
