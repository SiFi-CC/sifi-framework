#include <gtest/gtest-message.h>   // for Message
#include <gtest/gtest-test-part.h> // for TestPartResult
#include <gtest/gtest_pred_impl.h> // for SuiteApiResolver, ASSERT_EQ, ASSE...

#include "SRunContainer.h"

#include <ctime>  // for time, time_t
#include <memory> // for allocator

TEST(tests_database_run_container, basic_test)
{
    SRunContainer run1;
    run1.setId(13);
    std::time_t t1 = std::time(nullptr);
    std::time_t t2 = std::time(nullptr) + 1;
    run1.setStart(t1);
    run1.setStop(t2);
    run1.setType(7);

    auto [id, sta, sto, type] = run1.getData();
    ASSERT_EQ(id, 13);
    ASSERT_EQ(sta, t1);
    ASSERT_EQ(sto, t2);
    ASSERT_EQ(type, 7);
}
