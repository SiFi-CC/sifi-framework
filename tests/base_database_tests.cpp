#include <cppunit/extensions/HelperMacros.h>

#include "SContainer.h"
#include "SDatabase.h"
#include "SFibersCalibratorPar.h"
#include "SFibersGeomPar.h"
#include "SFibersLookup.h"
#include "SLookup.h"
#include "SParAsciiSource.h"
#include "SiFi.h"

class base_database_tests : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(base_database_tests);
    CPPUNIT_TEST(ascii_source_test);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;

protected:
    void ascii_source_test();
    void db_ascii_source_test();

private:
    SDatabase* db{nullptr};
    SParAsciiSource* ascii{nullptr};
};

CPPUNIT_TEST_SUITE_REGISTRATION(base_database_tests);

void base_database_tests::setUp()
{
    sifi()->disableAssertations();

    db = SDatabase::instance();
    db->addParContainer("FibersGeomPar", std::make_unique<SFibersGeomPar>());
    db->addLookupContainer("FibersDDLookupTable", std::make_unique<SFibersLookupTable>(
                                                      "FibersDDLookupTable", 0x1000, 0x1fff, 32));
    db->addLookupContainer("LookupTableMissing", std::make_unique<SFibersLookupTable>(
                                                     "LookupTableMissing", 0x2000, 0x2fff, 32));
    db->addCalContainer("FibersCalibratorPar",
                        std::make_unique<SFibersCalibratorPar>("FibersCalibratorPar"));
    db->addCalContainer("CalibratorParMissing",
                        std::make_unique<SFibersCalibratorPar>("CalibratorParMissing"));

    ascii = new SParAsciiSource("params.txt");
    // ascii->print();

    db->addSource(ascii);
}

void base_database_tests::tearDown() { delete ascii; }

void base_database_tests::ascii_source_test()
{
    // Test parameter containers
    CPPUNIT_ASSERT(ascii->getContainer("Cont1") == nullptr);
    CPPUNIT_ASSERT(ascii->getContainer("Cont2") == nullptr);
    CPPUNIT_ASSERT(ascii->getContainer("FibersGeomPar") != nullptr);

    CPPUNIT_ASSERT(db->getParContainer("Cont1") == nullptr);
    CPPUNIT_ASSERT(db->getParContainer("Cont2") == nullptr);

    auto* pFibersGeomPar = dynamic_cast<SFibersGeomPar*>(db->getParContainer("FibersGeomPar"));
    CPPUNIT_ASSERT(pFibersGeomPar != nullptr);

    CPPUNIT_ASSERT(pFibersGeomPar->getModules() == 2);
    CPPUNIT_ASSERT(pFibersGeomPar->getLayers(0) == 10);
    CPPUNIT_ASSERT(pFibersGeomPar->getLayers(1) == 30);

    // Test lookup containers
    CPPUNIT_ASSERT(db->getLookupContainer("Cont1") == nullptr);
    CPPUNIT_ASSERT(db->getLookupContainer("Cont2") == nullptr);

    auto* pFibersLookup =
        dynamic_cast<SFibersLookupTable*>(db->getLookupContainer("FibersDDLookupTable"));
    CPPUNIT_ASSERT(pFibersLookup != nullptr);
    CPPUNIT_ASSERT(db->getLookupContainer("FibersLookupTableMissing") == nullptr);

    CPPUNIT_ASSERT(pFibersLookup->getAddress(0x1000, 0) != nullptr);
    CPPUNIT_ASSERT(pFibersLookup->getAddress(0x1000, 1) != nullptr);
    CPPUNIT_ASSERT(pFibersLookup->getAddress(0x2000, 0) == nullptr);

    // Test cal containers
    CPPUNIT_ASSERT(db->getCalContainer("Cont1") == nullptr);
    CPPUNIT_ASSERT(db->getCalContainer("Cont2") == nullptr);

    auto* pCalibPar =
        dynamic_cast<SFibersCalibratorPar*>(db->getCalContainer("FibersCalibratorPar"));
    CPPUNIT_ASSERT(pCalibPar != nullptr);
    CPPUNIT_ASSERT(db->getCalContainer("CalibratorParMissing") == nullptr);

    SFibersChannel lc;
    lc.m = 0;
    lc.l = 0;
    lc.s = 0;
    lc.side = 'l';

    CPPUNIT_ASSERT(pCalibPar->getPar(&lc) != nullptr);

    lc.s = 1;
    CPPUNIT_ASSERT(pCalibPar->getPar(&lc) != nullptr);

    lc.m = 10;
    lc.s = 0;
    CPPUNIT_ASSERT(pCalibPar->getPar(&lc) == nullptr);
}
