set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(cppunit)

file(GLOB UnitTests_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests_*.cpp" )

configure_file(tests_config.h.in tests_config.h)

add_executable(UnitTester test_runner.cpp ${UnitTests_SRCS})
target_link_libraries(UnitTester PUBLIC SiFi SiFiTools Fibers Geant ${CPPUNIT_LIBRARIES} ROOT::RIO ROOT::Physics)

enable_testing()

foreach(test ${UnitTests_SRCS})
    get_filename_component(TestName ${test} NAME_WE)
    add_test(
        NAME ${TestName}
        COMMAND UnitTester ${TestName}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests
    )
endforeach(test)
