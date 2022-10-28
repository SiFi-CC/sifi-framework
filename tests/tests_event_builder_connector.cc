#include "SEventBuilderConnector.h"
#include "SRun.h"

#include <vector>

#include "cpr/cpr.h"
#include <gtest/gtest.h>

TEST(tests_event_builder_connector, run_insert)
{
    const char* eb_connectorurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(eb_connectorurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    SEventBuilderConnector eb_connector;

    auto type = 13;
    auto start_time = std::chrono::system_clock::now();
    auto file_name = "file1.txt";

    /** opening run */
    auto run = eb_connector.openRun(start_time, type, file_name);
    ASSERT_TRUE(run);

    /** closing run */
    auto stop_time = std::chrono::system_clock::now();
    auto run2 = eb_connector.closeRun(stop_time);
    ASSERT_TRUE(run2);

    /** closing run second time */
    auto run3 = eb_connector.closeRun(stop_time);
    ASSERT_FALSE(run3);

    /** opening run */
    start_time = std::chrono::system_clock::now();
    auto run4 = eb_connector.openRun(start_time, type, file_name);
    ASSERT_TRUE(run4);

    /** closing run */
    stop_time = std::chrono::system_clock::now();
    auto run5 = eb_connector.closeRun(stop_time);
    ASSERT_TRUE(run5);
}
