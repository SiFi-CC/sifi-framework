#include <gtest/gtest.h>

#include "SContainer.h"
#include "SDatabase.h"
#include "SFibersGeomPar.h"
#include "SParDatabaseSource.h"
#include "SiFi.h"

class tests_database_db_source : public ::testing::Test
{
protected:
    void SetUp() override
    {
        SRuntimeDb::init(&db);
        rdb()->setExperiment("TEST1");
        rdb()->addContainer("FibersGeomPar", []() { return new SFibersGeomPar; });
    }
    void TearDown() override { sifi()->enableAssertations(); }

    SDatabase db;
};

TEST_F(tests_database_db_source, get_run)
{
    auto dbapi = SIFI::make_database_source();

    // only runs 1-10 exists
    // ASSERT_EQ(dbapi->getRun(0), nullptr);
    // ASSERT_NE(dbapi->getRun(1), nullptr);
    // ASSERT_NE(dbapi->getRun(2), nullptr);
    // ASSERT_NE(dbapi->getRun(3), nullptr);
    // ASSERT_NE(dbapi->getRun(9), nullptr);
    // ASSERT_NE(dbapi->getRun(10), nullptr);
    // ASSERT_EQ(dbapi->getRun(11), nullptr);
    // ASSERT_EQ(dbapi->getRun(15), nullptr);
}

TEST_F(tests_database_db_source, get_container)
{
    auto dbapi = SIFI::make_database_source();

    ASSERT_EQ(dbapi->getContainer("Container1", 0), nullptr);
    ASSERT_NE(dbapi->getContainer("Container1", 3), nullptr);
    ASSERT_EQ(dbapi->getContainer("Container2", 0), nullptr);
    ASSERT_EQ(dbapi->getContainer("FibersGeomPar", 4), nullptr);
    ASSERT_EQ(dbapi->getContainer("FibersGeomPar", 4), nullptr);
}

// TEST_F(tests_database_db_source, dbapi_source_test)
// {
//     // Test parameter containers
//     ASSERT_EQ(dbapi->getContainer("Cont1", 0), nullptr);
//     ASSERT_EQ(dbapi->getContainer("Cont2", 0), nullptr);
//     ASSERT_NE(dbapi->getContainer("FibersGeomPar", 0), nullptr);
//
//     ASSERT_EQ(rdb()->getParContainer("Cont1"), nullptr);
//     ASSERT_EQ(rdb()->getParContainer("Cont2"), nullptr);
//
//     auto* pFibersGeomPar =
//     dynamic_cast<SFibersGeomPar*>(rdb()->getParContainer("FibersGeomPar"));
//     ASSERT_NE(pFibersGeomPar, nullptr);
//
//     ASSERT_EQ(pFibersGeomPar->getModules(), 2);
//     ASSERT_EQ(pFibersGeomPar->getLayers(0), 10);
//     ASSERT_EQ(pFibersGeomPar->getLayers(1), 30);
//
//     // Test lookup containers
//     ASSERT_EQ(rdb()->getLookupContainer("Cont1"), nullptr);
//     ASSERT_EQ(rdb()->getLookupContainer("Cont2"), nullptr);
//
//     auto* pFibersLookup =
//         dynamic_cast<SFibersLookupTable*>(rdb()->getLookupContainer("FibersDDLookupTable"));
//     ASSERT_NE(pFibersLookup, nullptr);
//     ASSERT_EQ(rdb()->getLookupContainer("FibersLookupTableMissing"), nullptr);
//
//     ASSERT_NE(pFibersLookup->getAddress(0x1000, 0), nullptr);
//     ASSERT_NE(pFibersLookup->getAddress(0x1000, 1), nullptr);
//     ASSERT_EQ(pFibersLookup->getAddress(0x2000, 0), nullptr);
//
//     // Test cal containers
//     ASSERT_EQ(rdb()->getCalContainer("Cont1"), nullptr);
//     ASSERT_EQ(rdb()->getCalContainer("Cont2"), nullptr);
//
//     auto* pCalibPar =
//         dynamic_cast<SFibersCalibratorPar*>(rdb()->getCalContainer("FibersCalibratorPar"));
//     ASSERT_NE(pCalibPar, nullptr);
//     ASSERT_EQ(rdb()->getCalContainer("CalibratorParMissing"), nullptr);
//
//     SFibersChannel lc;
//     lc.m = 0;
//     lc.l = 0;
//     lc.s = 0;
//     lc.side = 'l';
//
//     ASSERT_NE(pCalibPar->getPar(&lc), nullptr);
//
//     lc.s = 1;
//     ASSERT_NE(pCalibPar->getPar(&lc), nullptr);
//
//     lc.m = 10;
//     lc.s = 0;
//     ASSERT_EQ(pCalibPar->getPar(&lc), nullptr);
// }
