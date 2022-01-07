#include "SMysqlInterface.h"
#include <TObjArray.h>
#include <gtest/gtest.h>

#include "cpr/cpr.h"

TEST(tests_database_mysql_interface, connection_tests)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::pair<std::string, bool>> inputs = {{apiurl, true},
                                                        {"http://localhost:800000", false}};

    for (auto& i : inputs)
    {
        SMysqlInterface url(i.first, token);
        EXPECT_TRUE(url.connected() == i.second) << " on input " << i.first;
    }
}

TEST(tests_database_mysql_interface, auth_tests)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    std::vector<std::pair<std::string, bool>> inputs = {
        {token, true}, {"0123456789", false}, {"deadbeef0a", false}};

    for (auto& i : inputs)
    {
        SMysqlInterface url(apiurl, i.first);
        EXPECT_TRUE(url.connected() == i.second) << " on input " << i.first;
    }
}

TEST(tests_database_mysql_interface, experiment_request)
{
    const char* apiurl = getenv("SIFIDBAPI");
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(apiurl, nullptr);
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api(apiurl, token);

    auto rel_cont = api.getExperimentContainer("TEST");
    EXPECT_TRUE(rel_cont.has_value());
    EXPECT_STREQ(rel_cont.value().name.c_str(), "TEST");
    EXPECT_EQ(rel_cont.value().first_run, 3);
    EXPECT_EQ(rel_cont.value().last_run, 9);
    rel_cont = api.getExperimentContainer("BAD_TEST");
    EXPECT_FALSE(rel_cont.has_value());
    rel_cont = api.getExperimentContainer("");
    EXPECT_FALSE(rel_cont.has_value());
}

TEST(tests_database_mysql_interface, run_request)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    auto run_cont = api.getRunContainer(1);
    EXPECT_EQ(run_cont.runid, 1);
    run_cont = api.getRunContainer(2);
    EXPECT_EQ(run_cont.runid, 2);
    run_cont = api.getRunContainer(3);
    EXPECT_EQ(run_cont.runid, 3);
    run_cont = api.getRunContainer(40);
    EXPECT_EQ(run_cont.runid, 40);
}

TEST(tests_database_mysql_interface, run_range_request)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    auto run_cont = api.getRunContainers(1, 2);
    run_cont = api.getRunContainers(1, 3);
    run_cont = api.getRunContainers(2, 3);
    run_cont = api.getRunContainers(1, 0);
}

TEST(tests_database_mysql_interface, run_range_from_experiment)
{
    /* Test whether you can get runs by experiment name. The TEST experiment contains
     * runs 3-9 which is 7 entities.
     */
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    api.setExperiment("TEST");

    auto run_cont = api.getRunContainers(0, 0);
    EXPECT_EQ(run_cont.size(), 7);
}

TEST(tests_database_mysql_interface, container_find)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    api.setExperiment("TEST"); // runs 3-9

    // GeomPar, params 3-7 are validated, 8-9 are not validated
    auto cont = api.findContainer("FibersGeomPar");
    EXPECT_TRUE(cont); // params for runs 1 are not existing (utside test)

    cont = api.findContainer("BadFibersGeomPar");
    EXPECT_FALSE(cont); // params for runs 1 are not existing (utside test)

    api.setExperiment("BAD_TEST"); // runs 3-9

    // GeomPar, params 3-7 are validated, 8-9 are not validated
    cont = api.findContainer("FibersGeomPar");
    EXPECT_FALSE(cont); // params for runs 1 are not existing (utside test)
}

TEST(tests_database_mysql_interface, container_request)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    api.setExperiment("TEST"); // runs 3-9

    // GeomPar, params 3-7 are validated, 8-9 are not validated
    auto cont = api.getContainer("FibersGeomPar", 1);
    EXPECT_FALSE(cont.has_value()); // params for runs 1 are not existing (utside test)
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 3);

    cont = api.getContainer("FibersGeomPar", 2);
    EXPECT_FALSE(cont.has_value()); // same as above
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 3);

    cont = api.getContainer("FibersGeomPar", 3);
    EXPECT_TRUE(cont.has_value()); // params exist and validated
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 3);

    cont = api.getContainer("FibersGeomPar", 8);
    EXPECT_FALSE(cont.has_value()); // params for runs 8-9 are not validated
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 8);

    // CalibratorPar
    cont = api.getContainer("FibersCalibratorPar", 3);
    EXPECT_TRUE(cont.has_value()); // params for runs 1 are not existing (utside test)
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 3);

    cont = api.getContainer("FibersCalibratorPar", 4);
    EXPECT_TRUE(cont.has_value()); // same as above
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 4);

    cont = api.getContainer("FibersCalibratorPar", 7);
    EXPECT_TRUE(cont.has_value()); // params exist and validated
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 7);

    cont = api.getContainer("FibersCalibratorPar", 8);
    EXPECT_FALSE(cont.has_value()); // params for runs 8-9 are not validated
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 8);

    // DigitizerPar
    cont = api.getContainer("FibersDigitizerPar", 3);
    EXPECT_FALSE(cont.has_value()); // params for runs 1 are not existing (utside test)
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 3);

    cont = api.getContainer("FibersDigitizerPar", 4);
    EXPECT_TRUE(cont.has_value()); // same as above
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 4);

    cont = api.getContainer("FibersDigitizerPar", 7);
    EXPECT_FALSE(cont.has_value()); // params exist and validated
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 7);

    cont = api.getContainer("FibersDigitizerPar", 8);
    EXPECT_TRUE(cont.has_value()); // params for runs 8-9 are not validated
    if (cont.has_value()) EXPECT_LE(cont->validity.from, 8);
}

TEST(tests_database_mysql_interface, container_range_request)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    api.setExperiment("TEST");

    // GeomPar
    auto cont = api.getContainers("FibersGeomPar", 1, 3);
    EXPECT_EQ(cont.size(), 0);
    cont = api.getContainers("FibersGeomPar", 2, 3);
    EXPECT_EQ(cont.size(), 0);
    cont = api.getContainers("FibersGeomPar", 3, 4);
    EXPECT_EQ(cont.size(), 1);
    cont = api.getContainers("FibersGeomPar", 1, 2);
    EXPECT_EQ(cont.size(), 0);
    cont = api.getContainers("FibersGeomPar", 2, 0);
    EXPECT_EQ(cont.size(), 1); // only one bot second set is not validated

    cont = api.getContainers("FibersCalibratorPar", 2, 0);
    EXPECT_EQ(cont.size(), 2); // only one bot second set is not validated

    cont = api.getContainers("FibersDigitizerPar", 2, 0);
    EXPECT_EQ(cont.size(), 2); // only one bot second set is not validated
}
