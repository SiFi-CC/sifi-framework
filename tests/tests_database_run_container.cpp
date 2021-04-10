#include <cppunit/extensions/HelperMacros.h>

#include "SRunContainer.h"


class tests_database_run_container : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(tests_database_run_container);
    CPPUNIT_TEST(run_container_test);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
    void run_container_test();

private:

};

CPPUNIT_TEST_SUITE_REGISTRATION(tests_database_run_container);

void tests_database_run_container::setUp() {}

void tests_database_run_container::tearDown() {}

void tests_database_run_container::run_container_test()
{
    SRunContainer run1;
    run1.setId(13);
    std::time_t t1 = std::time(nullptr);
    std::time_t t2 = std::time(nullptr) + 1;
    run1.setStart(t1);
    run1.setStop(t2);
    run1.setType(7);

    auto [id, sta, sto, type] = run1.getData();
    CPPUNIT_ASSERT(id == 13);
    CPPUNIT_ASSERT(sta == t1);
    CPPUNIT_ASSERT(sto == t2);
    CPPUNIT_ASSERT(type == 7);
}
