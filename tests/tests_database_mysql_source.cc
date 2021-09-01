#include <gtest/gtest.h>

#include "SContainer.h"
#include "SDatabase.h"
#include "SFibersGeomPar.h"
#include "SParDatabaseSource.h"
#include "SiFi.h"

class tests_database_mysql_source : public ::testing::Test
{
protected:
    void SetUp() override
    {
        db = SDatabase::instance();
        db->setRelease("TEST");

        db->addParContainer("FibersGeomPar", std::make_unique<SFibersGeomPar>());
        //         db->addLookupContainer(
        //             "FibersDDLookupTable",
        //             std::make_unique<SFibersLookupTable>("FibersDDLookupTable", 0x1000, 0x1fff,
        //             32));
        //         db->addLookupContainer("LookupTableMissing",
        //         std::make_unique<SFibersLookupTable>(
        //                                                          "LookupTableMissing", 0x2000,
        //                                                          0x2fff, 32));
        //         db->addCalContainer("FibersCalibratorPar",
        //                             std::make_unique<SFibersCalibratorPar>("FibersCalibratorPar"));
        //         db->addCalContainer("CalibratorParMissing",
        //                             std::make_unique<SFibersCalibratorPar>("CalibratorParMissing"));

        mysql = new SParDatabaseSource();
        //         mysql->print();

        db->addSource(mysql);
    }
    void TearDown() override
    {
        delete mysql;
        sifi()->enableAssertations();
    }

    SDatabase* db{nullptr};
    SParDatabaseSource* mysql{nullptr};
};

TEST_F(tests_database_mysql_source, mysql_source_test)
{
    ASSERT_EQ(mysql->getContainer("Cont1", 0), nullptr);
    ASSERT_EQ(mysql->getContainer("Cont2", 0), nullptr);
    ASSERT_NE(mysql->getContainer("FibersGeomPar", 4), nullptr);
    ASSERT_NE(mysql->getContainer("FibersGeomPar", 4), nullptr);
}

// TEST_F(tests_database_mysql_source, mysql_source_test)
// {
//     // Test parameter containers
//     ASSERT_EQ(mysql->getContainer("Cont1", 0), nullptr);
//     ASSERT_EQ(mysql->getContainer("Cont2", 0), nullptr);
//     ASSERT_NE(mysql->getContainer("FibersGeomPar", 0), nullptr);
//
//     ASSERT_EQ(db->getParContainer("Cont1"), nullptr);
//     ASSERT_EQ(db->getParContainer("Cont2"), nullptr);
//
//     auto* pFibersGeomPar = dynamic_cast<SFibersGeomPar*>(db->getParContainer("FibersGeomPar"));
//     ASSERT_NE(pFibersGeomPar, nullptr);
//
//     ASSERT_EQ(pFibersGeomPar->getModules(), 2);
//     ASSERT_EQ(pFibersGeomPar->getLayers(0), 10);
//     ASSERT_EQ(pFibersGeomPar->getLayers(1), 30);
//
//     // Test lookup containers
//     ASSERT_EQ(db->getLookupContainer("Cont1"), nullptr);
//     ASSERT_EQ(db->getLookupContainer("Cont2"), nullptr);
//
//     auto* pFibersLookup =
//         dynamic_cast<SFibersLookupTable*>(db->getLookupContainer("FibersDDLookupTable"));
//     ASSERT_NE(pFibersLookup, nullptr);
//     ASSERT_EQ(db->getLookupContainer("FibersLookupTableMissing"), nullptr);
//
//     ASSERT_NE(pFibersLookup->getAddress(0x1000, 0), nullptr);
//     ASSERT_NE(pFibersLookup->getAddress(0x1000, 1), nullptr);
//     ASSERT_EQ(pFibersLookup->getAddress(0x2000, 0), nullptr);
//
//     // Test cal containers
//     ASSERT_EQ(db->getCalContainer("Cont1"), nullptr);
//     ASSERT_EQ(db->getCalContainer("Cont2"), nullptr);
//
//     auto* pCalibPar =
//         dynamic_cast<SFibersCalibratorPar*>(db->getCalContainer("FibersCalibratorPar"));
//     ASSERT_NE(pCalibPar, nullptr);
//     ASSERT_EQ(db->getCalContainer("CalibratorParMissing"), nullptr);
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
//
//
// TEST(tests_cpr, basic_tests)
// {
//     cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
//                                cpr::Authentication{"user", "pass"},
//                                cpr::Parameters{{"anon", "true"}, {"key", "value"}});
//     ASSERT_EQ(r.status_code, 200); // 200
//     ASSERT_EQ(r.header["content-type"], "application/json; charset=utf-8");
//     // printf(r.text.c_str());
//     SMysqlInterface api("http://127.0.0.1:9081/api/");
//     auto run_cont = api.getRunContainer(1);
//     run_cont->print();
//     // api.addRunContainer(std::move(run_cont.value()));
//     auto array = api.getRunContainers(1, 2);
//     for (auto v : array)
//     {
//         auto x = (SRunContainer*)v;
//         x->print();
//     }
//     auto s_cont = api.getContainer("SFibersStackGeomPar", 1);
//     s_cont->print("SFibersStackGeomPar");
// }
