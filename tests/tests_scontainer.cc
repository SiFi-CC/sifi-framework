#include <gtest/gtest.h>

#include "SContainer.h"

#include <string>      // for string
#include <sys/types.h> // for time_t

TEST(tests_scontainer, valid_range_test)
{
    validity_range_t r1 = {100, 200};
    validity_range_t r2 = {150, 250};
    validity_range_t r3 = {225, 450};

    std::time_t t1 = 100;
    std::time_t t2 = 120;
    std::time_t t3 = 220;

    ASSERT_LT(r1, r2);
    ASSERT_FALSE(r1 < t1);
    ASSERT_LT(r1, t2);
    ASSERT_LT(r1, t3);
    ASSERT_EQ(r1, t1);
    ASSERT_EQ(r1, t2);
    ASSERT_FALSE(r1 == t3);

    ASSERT_TRUE(r2.check_overlap(r1));
    ASSERT_FALSE(r3.check_overlap(r1));
    ASSERT_TRUE(r3.check_overlap(r2));
}
