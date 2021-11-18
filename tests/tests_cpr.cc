#include <gtest/gtest.h>

#include "cpr/cpr.h"

TEST(tests_cpr, basic_tests)
{
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                               cpr::Authentication{"user", "pass"},
                               cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    ASSERT_EQ(r.status_code, 200); // 200
    ASSERT_EQ(r.header["content-type"], "application/json; charset=utf-8");
    // printf(r.text.c_str());
}
