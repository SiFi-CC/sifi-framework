#include "SRESTInterface.h"
#include <TObjArray.h>
#include <gtest/gtest.h>

#include "cpr/cpr.h"

#include <vector>

TEST(tests_database_rest_interface, connection_tests)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::pair<std::string, bool>> inputs = {{apiurl, true},
                                                        {"http://localhost:800000", false}};

    for (auto& i : inputs)
    {
        SRESTInterface url(i.first, token);
        EXPECT_TRUE(url.connected() == i.second) << " on input " << i.first;
    }
}

TEST(tests_database_rest_interface, auth_tests)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::pair<std::string, bool>> inputs = {
        {token, true}, {"0123456789", false}, {"deadbeef0a", false}};

    for (auto& i : inputs)
    {
        SRESTInterface url(apiurl, i.first);
        EXPECT_TRUE(url.connected() == i.second) << " on input " << i.first;
    }
}

TEST(tests_database_rest_interface, experiment_request)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    SRESTInterface api(apiurl, token);

    auto rel_cont = api.getExperimentContainer("TEST1");
    EXPECT_TRUE(rel_cont.has_value());
    EXPECT_STREQ(rel_cont.value().name.c_str(), "TEST1");
    // EXPECT_EQ(rel_cont.value().first_run, 1003);
    // EXPECT_EQ(rel_cont.value().last_run, 1009);
    rel_cont = api.getExperimentContainer("BAD_TEST");
    EXPECT_FALSE(rel_cont.has_value());
    rel_cont = api.getExperimentContainer("");
    EXPECT_FALSE(rel_cont.has_value());
}

TEST(tests_database_rest_interface, run_request)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<int> inputs = {1, 2, 3, 4};

    SRESTInterface api(apiurl, token);
    for (auto& i : inputs)
    {
        EXPECT_EQ(api.getRunContainer(i).id, i) << " for input " << i;
    }
}

TEST(tests_database_rest_interface, run_range_request)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    SRESTInterface api(apiurl, token);
    auto run_cont = api.getRunContainers(1, 2);
    run_cont = api.getRunContainers(1, 3);
    run_cont = api.getRunContainers(2, 3);
    run_cont = api.getRunContainers(1, 0);
}

TEST(tests_database_rest_interface, run_range_from_experiment)
{
    /* Test whether you can get runs by experiment name. The TEST experiment contains
     * runs 3-9 which is 7 entities.
     */
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    SRESTInterface api(apiurl, token);
    api.setExperiment("TEST1");

    auto run_cont = api.getRunContainers(0, 0);
    EXPECT_EQ(run_cont.size(), 10);
}

TEST(tests_database_rest_interface, run_insert)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    SRESTInterface api(apiurl, token);

    auto type = 13;
    auto start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto file_name = "file1.txt";

    /** opening run */
    auto run = api.openRunContainer(type, start_time, file_name);
    ASSERT_TRUE(run);
    ASSERT_EQ(run->type, type);
    ASSERT_EQ(run->start_time, start_time);
    ASSERT_EQ(run->stop_time, 0);
    ASSERT_EQ(run->status, SRun::Status::Invalid);

    /** closing run */
    auto stop_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto run2 = api.closeRunContainer(stop_time);
    ASSERT_TRUE(run2);
    ASSERT_EQ(run2->type, run->type);
    ASSERT_EQ(run2->start_time, run->start_time);
    ASSERT_EQ(run2->stop_time, stop_time);
    ASSERT_EQ(run2->status, SRun::Status::Invalid);

    /** closing run second time */
    auto run3 = api.closeRunContainer(stop_time);
    ASSERT_FALSE(run3);

    /** opening run */
    auto run4 = api.openRunContainer(type, start_time, file_name);
    ASSERT_TRUE(run4);
    ASSERT_EQ(run4->type, type);
    ASSERT_EQ(run4->start_time, start_time);
    ASSERT_EQ(run4->stop_time, 0);
    ASSERT_EQ(run4->status, SRun::Status::Invalid);

    /** closing run */
    stop_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto run5 = api.closeRunContainer(stop_time);
    ASSERT_TRUE(run5);
    ASSERT_EQ(run5->type, run->type);
    ASSERT_EQ(run5->start_time, run->start_time);
    ASSERT_EQ(run5->stop_time, stop_time);
    ASSERT_EQ(run5->status, SRun::Status::Invalid);
}

TEST(tests_database_rest_interface, container_find)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::tuple<std::string, std::string, bool>> inputs = {
        {"TEST1", "Container1", true},
        {"TEST1", "Container10", false},
        {"BAD_TEST", "Container1", false},
        {"BAD_TEST", "Container10", false},
    };

    SRESTInterface api(apiurl, token);
    for (auto const& i : inputs)
    {
        api.setExperiment(std::get<0>(i));
        auto cont = api.findContainer(std::get<1>(i));
        EXPECT_EQ(cont, std::get<2>(i))
            << " for EXP: " << std::get<0>(i) << " CONT: " << std::get<1>(i);
    }
}

TEST(tests_database_rest_interface, container_request)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::tuple<std::string, int, bool, int>> inputs = {
        // Container1, params 3-7 are validated, 8-9 are not validated
        {"Container1", 2, false, 4}, // params for runs 1 are not existing (utside test)
        {"Container1", 3, false, 4}, // same as above
        {"Container1", 4, true, 4},  // params exist and validated
        {"Container1", 9, false, 9}, // params for runs 8-9 are not validated
        {"Container2", 4, true, 4},  // params for runs 1 are not existing (utside test)
        {"Container2", 5, true, 5},  // same as above
        {"Container2", 8, true, 8},  // params exist and validated
        {"Container2", 9, false, 9}, // params for runs 8-9 are not validated
        {"Container3", 4, false, 4}, // params for runs 1 are not existing (utside test)
        {"Container3", 5, true, 5},  // same as above
        {"Container3", 8, false, 8}, // params do not exist
        {"Container3", 9, false, 9}  // params for runs 9-10 are not validated
    };

    SRESTInterface api(apiurl, token);
    api.setExperiment("TEST1");

    for (auto const& i : inputs)
    {

        auto cont = api.getContainer(std::get<0>(i), std::get<1>(i));
        EXPECT_EQ(cont.has_value(), std::get<2>(i))
            << " for CONT: " << std::get<0>(i) << " RUN: " << std::get<1>(i);
        if (cont.has_value())
            EXPECT_LE(cont->validity.from, std::get<3>(i))
                << " for CONT: " << std::get<0>(i) << " RUN: " << std::get<1>(i) << " validity LE "
                << std::get<3>(i);
    }
}

TEST(tests_database_rest_interface, container_range_request)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::tuple<std::string, int, int, int>> inputs = {
        {"Container1", 2, 4, 0}, //
        {"Container1", 3, 4, 0}, //
        {"Container1", 4, 5, 1}, //
        {"Container1", 2, 3, 0}, //
        {"Container1", 3, 1, 1}, // only one bot second set is not validated
        {"Container2", 3, 1, 2}, // only one bot second set is not validated
        {"Container3", 3, 1, 2}, // only one bot second set is not validated
    };

    SRESTInterface api(apiurl, token);
    api.setExperiment("TEST1");

    for (auto const& i : inputs)
    {
        auto cont = api.getContainers(std::get<0>(i), std::get<1>(i), std::get<2>(i));
        EXPECT_EQ(cont.size(), std::get<3>(i))
            << " for CONT: " << std::get<0>(i) << " RANGE: " << std::get<1>(i) << " to "
            << std::get<2>(i);
    }
}
