#include <cppunit/extensions/HelperMacros.h>

#include "SContainer.h"

#include <ctime>

class tests_scontainer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(tests_scontainer);
    CPPUNIT_TEST(valid_range_test);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
    void valid_range_test();

private:
};

CPPUNIT_TEST_SUITE_REGISTRATION(tests_scontainer);

void tests_scontainer::setUp() {}

void tests_scontainer::tearDown() {}

void tests_scontainer::valid_range_test()
{
    validity_range_t r1 = {100, 200};
    validity_range_t r2 = {150, 250};
    validity_range_t r3 = {225, 450};

    std::time_t t1 = 100;
    std::time_t t2 = 120;
    std::time_t t3 = 220;

    CPPUNIT_ASSERT(r1 < r2);
    CPPUNIT_ASSERT(!(r1 < t1));
    CPPUNIT_ASSERT(r1 < t2);
    CPPUNIT_ASSERT(r1 < t3);
    CPPUNIT_ASSERT(r1 == t1);
    CPPUNIT_ASSERT(r1 == t2);
    CPPUNIT_ASSERT(!(r1 == t3));

    CPPUNIT_ASSERT(r2.check_overlap(r1) == true);
    CPPUNIT_ASSERT(r3.check_overlap(r1) == false);
    CPPUNIT_ASSERT(r3.check_overlap(r2) == true);
}
