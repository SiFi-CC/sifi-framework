#include <gtest/gtest.h>

#include "tests/tests_config.h"

#include "SCalContainer.h"
#include "SContainer.h"
#include "SDatabase.h"
#include "SDetectorManager.h"
#include "SFibersDetector.h"
#include "SFibersGeomPar.h"
#include "SParAsciiSource.h"
#include "SParDatabaseSource.h"
#include "SParRootSource.h"
#include "SiFi.h"

class tests_database : public ::testing::Test
{
protected:
    void SetUp() override { sifi()->disableAssertations(); }

    void TearDown() override { sifi()->enableAssertations(); }
};

TEST_F(tests_database, ascii_source_test)
{

    SDatabase db;
    SRuntimeDb::init(&db);
    rdb()->setExperiment("TEST");

    auto source = SIFI::make_ascii_source(tests_path + "params_ascii.txt");
    ASSERT_NE(source->getContainer("FibersCalibratorPar", 0), nullptr);
    rdb()->addSource(std::move(source));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->initContainers(0);
    // SRuntimeDb::get()->print();

    //     auto rdb()->getContainer();
    auto ptr = SRuntimeDb::get()->getCalContainer("FibersCalibratorPar");
    ASSERT_NE(ptr, nullptr);

    auto pCalibratorPar = dynamic_cast<SCalContainer<6>*>(ptr);

    ASSERT_NE(pCalibratorPar, nullptr);
}

TEST_F(tests_database, root_source_test)
{
    SDatabase db;
    SRuntimeDb::init(&db);
    sifi()->disableAssertations();
    rdb()->setExperiment("TEST");
    auto source = SIFI::make_root_source(tests_path + "params_root.root");
    rdb()->addSource(std::move(source));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->initContainers(0);

    //     auto rdb()->getContainer();
    auto pCalibratorPar =
        dynamic_cast<SCalContainer<6>*>(SRuntimeDb::get()->getCalContainer("FibersCalibratorPar"));

    ASSERT_NE(pCalibratorPar, nullptr);
}

TEST_F(tests_database, database_database_test)
{
    SDatabase db;
    SRuntimeDb::init(&db);
    sifi()->disableAssertations();
    rdb()->setExperiment("TEST");
    auto source = SIFI::make_database_source();
    rdb()->addSource(std::move(source));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->initContainers(0);

    //     auto rdb()->getContainer();
    auto pCalibratorPar =
        dynamic_cast<SCalContainer<6>*>(SRuntimeDb::get()->getCalContainer("FibersCalibratorPar"));

    ASSERT_NE(pCalibratorPar, nullptr);
}
