#include <gtest/gtest.h>

#include <tests/tests_config.h>

#include "SCalContainer.h"
#include "SContainer.h"
#include "SDatabase.h"
#include "SFibersCalibratorPar.h"
#include "SFibersGeomPar.h"
#include "SFibersLookup.h"
#include "SLookup.h"
#include "SPar.h"
#include "SParAsciiSource.h"
#include "SiFi.h"

#include <memory> // for allocator, make_unique
#include <string> // for operator+, string

class tests_database_ascii_source : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sifi()->disableAssertations();

        db = SDatabase::instance();
        db->addContainer("FibersGeomPar", []() { return new SFibersGeomPar; });
        db->addContainer(
            "FibersDDLookupTable",
            []() { return new SFibersLookupTable("FibersDDLookupTable", 0x1000, 0x1fff, 32); });
        db->addContainer(
            "LookupTableMissing",
            []() { return new SFibersLookupTable("LookupTableMissing", 0x2000, 0x2fff, 32); });
        db->addContainer("FibersCalibratorPar",
                            []() { return new SFibersCalibratorPar("FibersCalibratorPar"); });
        db->addContainer("CalibratorParMissing",
                            []() { return new SFibersCalibratorPar("CalibratorParMissing"); });

        ascii = new SParAsciiSource(tests_path + "params.txt");
        // ascii->print();

        db->addSource(ascii);
    }
    void TearDown() override
    {
        delete ascii;
        sifi()->enableAssertations();
    }

    SDatabase* db{nullptr};
    SParAsciiSource* ascii{nullptr};
};

TEST_F(tests_database_ascii_source, ascii_source_test)
{
    // Test parameter containers
    ASSERT_EQ(ascii->getContainer("Cont1", 0), nullptr);
    ASSERT_EQ(ascii->getContainer("Cont2", 0), nullptr);
    ASSERT_NE(ascii->getContainer("FibersGeomPar", 0), nullptr);

    ASSERT_EQ(db->getParContainer("Cont1"), nullptr);
    ASSERT_EQ(db->getParContainer("Cont2"), nullptr);

    auto* pFibersGeomPar = dynamic_cast<SFibersGeomPar*>(db->getParContainer("FibersGeomPar"));
    ASSERT_NE(pFibersGeomPar, nullptr);

    ASSERT_EQ(pFibersGeomPar->getModules(), 2);
    ASSERT_EQ(pFibersGeomPar->getLayers(0), 10);
    ASSERT_EQ(pFibersGeomPar->getLayers(1), 30);

    // Test lookup containers
    ASSERT_EQ(db->getLookupContainer("Cont1"), nullptr);
    ASSERT_EQ(db->getLookupContainer("Cont2"), nullptr);

    auto* pFibersLookup =
        dynamic_cast<SFibersLookupTable*>(db->getLookupContainer("FibersDDLookupTable"));
    ASSERT_NE(pFibersLookup, nullptr);
    ASSERT_EQ(db->getLookupContainer("FibersLookupTableMissing"), nullptr);

    ASSERT_NE(pFibersLookup->getAddress(0x1000, 0), nullptr);
    ASSERT_NE(pFibersLookup->getAddress(0x1000, 1), nullptr);
    ASSERT_EQ(pFibersLookup->getAddress(0x2000, 0), nullptr);

    // Test cal containers
    ASSERT_EQ(db->getCalContainer("Cont1"), nullptr);
    ASSERT_EQ(db->getCalContainer("Cont2"), nullptr);

    auto* pCalibPar =
        dynamic_cast<SFibersCalibratorPar*>(db->getCalContainer("FibersCalibratorPar"));
    ASSERT_NE(pCalibPar, nullptr);
    ASSERT_EQ(db->getCalContainer("CalibratorParMissing"), nullptr);

    SFibersChannel lc;
    lc.m = 0;
    lc.l = 0;
    lc.s = 0;
    lc.side = 'l';

    ASSERT_NE(pCalibPar->getPar(&lc), nullptr);

    lc.s = 1;
    ASSERT_NE(pCalibPar->getPar(&lc), nullptr);

    lc.m = 10;
    lc.s = 0;
    ASSERT_EQ(pCalibPar->getPar(&lc), nullptr);
}
