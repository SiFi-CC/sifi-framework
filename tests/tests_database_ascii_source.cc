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

        SRuntimeDb::init(&db);
        rdb()->addContainer("FibersGeomPar", []() { return new SFibersGeomPar; });
        rdb()->addContainer(
            "FibersLookupTable",
            []() { return new SFibersLookupTable("FibersLookupTable", 0x1000, 0x1fff, 32); });
        rdb()->addContainer(
            "LookupTableMissing",
            []() { return new SFibersLookupTable("LookupTableMissing", 0x2000, 0x2fff, 32); });
        rdb()->addContainer("FibersCalibratorPar",
                            []() { return new SFibersCalibratorPar("FibersCalibratorPar"); });
        rdb()->addContainer("CalibratorParMissing",
                            []() { return new SFibersCalibratorPar("CalibratorParMissing"); });

        // missing containers cannot be marked READ
        rdb()->setContainerMode("LookupTableMissing", SDatabase::ContainerMode::None);
        rdb()->setContainerMode("CalibratorParMissing", SDatabase::ContainerMode::None);

        ascii = new SParAsciiSource(tests_path + "params_ascii.txt");
        // ascii->print();

        rdb()->addSource(ascii);
        rdb()->initContainers();
        rdb()->print();
    }
    void TearDown() override
    {
        delete ascii;
        sifi()->enableAssertations();
    }

    SDatabase db;
    SParAsciiSource* ascii{nullptr};
};

TEST_F(tests_database_ascii_source, ascii_source_test)
{
    // Test parameter containers
    ASSERT_EQ(ascii->getContainer("MissingCont1", 0), nullptr);
    ASSERT_EQ(ascii->getContainer("MissingCont2", 0), nullptr);
    ASSERT_NE(ascii->getContainer("FibersGeomPar", 0), nullptr);

    ASSERT_EQ(rdb()->getParContainer("Cont1"), nullptr);
    ASSERT_EQ(rdb()->getParContainer("Cont2"), nullptr);

    auto* pFibersGeomPar = dynamic_cast<SFibersGeomPar*>(rdb()->getParContainer("FibersGeomPar"));
    ASSERT_NE(pFibersGeomPar, nullptr);

    ASSERT_EQ(pFibersGeomPar->getModules(), 2);
    ASSERT_EQ(pFibersGeomPar->getLayers(0), 10);
    ASSERT_EQ(pFibersGeomPar->getLayers(1), 30);

    // Test lookup containers
    ASSERT_EQ(rdb()->getLookupContainer("MissingCont1"), nullptr);
    ASSERT_EQ(rdb()->getLookupContainer("MissingCont2"), nullptr);

    auto* pFibersLookup =
        dynamic_cast<SFibersLookupTable*>(rdb()->getLookupContainer("FibersLookupTable"));
    ASSERT_NE(pFibersLookup, nullptr);
    ASSERT_EQ(rdb()->getLookupContainer("FibersLookupTableMissing"), nullptr);

    ASSERT_NE(pFibersLookup->getAddress(0x1000, 0), nullptr);
    ASSERT_NE(pFibersLookup->getAddress(0x1000, 1), nullptr);
    ASSERT_EQ(pFibersLookup->getAddress(0x2000, 0), nullptr);

    // Test cal containers
    ASSERT_EQ(rdb()->getCalContainer("MissingCont1"), nullptr);
    ASSERT_EQ(rdb()->getCalContainer("MissingCont2"), nullptr);

    auto* pCalibPar =
        dynamic_cast<SFibersCalibratorPar*>(rdb()->getCalContainer("FibersCalibratorPar"));
    ASSERT_NE(pCalibPar, nullptr);
    ASSERT_EQ(rdb()->getCalContainer("FibersCalibratorParMissing"), nullptr);

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
