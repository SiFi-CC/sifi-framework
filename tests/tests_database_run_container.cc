#include <gtest/gtest.h>

#include "SRun.h"

#include <ctime>  // for time, time_t
#include <memory> // for allocator

TEST(tests_database_run_container, basic_test)
{
    SRun run1;
    run1.id = 13;
    std::time_t t1 = std::time(nullptr);
    std::time_t t2 = std::time(nullptr) + 1;
    run1.start_time = t1;
    run1.stop_time = t2;
    run1.type = 7;

    ASSERT_EQ(run1.id, 13);
    ASSERT_EQ(run1.start_time, t1);
    ASSERT_EQ(run1.stop_time, t2);
    ASSERT_EQ(run1.type, 7);
    ASSERT_EQ(run1.status, SRun::Status::Invalid);
}
