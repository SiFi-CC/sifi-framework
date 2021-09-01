#include "SMysqlInterface.h"
#include <TObjArray.h>
#include <gtest/gtest.h>

#include "cpr/cpr.h"

TEST(tests_database_mysql_interface, connection_tests)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface url1("http://127.0.0.1:8000", token);
    EXPECT_TRUE(url1.connected());

    SMysqlInterface url2("http://127.0.0.1:8001", token);
    EXPECT_FALSE(url2.connected());

    SMysqlInterface url3("http://127.0.0.1:8002", token);
    EXPECT_FALSE(url3.connected());
}

TEST(tests_database_mysql_interface, auth_tests)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface url1("http://127.0.0.1:8000", "982675c1f8e6e5cddb08084adce759e925a82880");
    EXPECT_FALSE(url1.connected());

    SMysqlInterface url2("http://127.0.0.1:8000", token);
    EXPECT_TRUE(url2.connected());

    SMysqlInterface url3("http://127.0.0.1:8000", "982675c1f8e6e5cddb08084adce759e925a82881");
    EXPECT_FALSE(url3.connected());
}

TEST(tests_database_mysql_interface, run_request)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    auto run_cont = api.getRunContainer(1);
    EXPECT_TRUE(run_cont.has_value());
    run_cont = api.getRunContainer(2);
    EXPECT_TRUE(run_cont.has_value());
    run_cont = api.getRunContainer(3);
    EXPECT_TRUE(run_cont.has_value());
    run_cont = api.getRunContainer(40);
    EXPECT_FALSE(run_cont.has_value());
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

TEST(tests_database_mysql_interface, run_range_from_release)
{
    /* Test whether you can get runs by release name. The TEST release contains
     * runs 3-9 which is 7 entities.
     */
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    api.setParamRelease("TEST");

    auto run_cont = api.getRunContainers(0, 0);
    EXPECT_EQ(run_cont.GetEntries(), 7);
}

TEST(tests_database_mysql_interface, container_request)
{
    const char* token = getenv("SIFIAUTH");
    ASSERT_STRNE(token, nullptr);

    SMysqlInterface api("http://127.0.0.1:8000", token);
    api.setParamRelease("TEST"); // runs 3-9

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
    api.setParamRelease("TEST");

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
