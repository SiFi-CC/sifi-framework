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
    void SetUp() override
    {
        sifi()->disableAssertations();

        src_ascii = std::make_unique<SParAsciiSource>(tests_path + "params_ascii.txt");
        src_root = std::make_unique<SParRootSource>(tests_path + "params_root.root");
        src_mysql = std::make_unique<SParDatabaseSource>();
    }
    void TearDown() override
    {
        src_ascii.release();
        src_root.release();
        src_mysql.release();
    }

    std::unique_ptr<SParAsciiSource> src_ascii;
    std::unique_ptr<SParRootSource> src_root;
    std::unique_ptr<SParDatabaseSource> src_mysql{nullptr};
};

TEST_F(tests_database, ascii_source_test)
{
    ASSERT_NE(src_ascii->getContainer("FibersCalibratorPar", 0), nullptr);

    SDatabase db;
    SRuntimeDb::init(&db);
    rdb()->setExperiment("TEST");
    rdb()->addSource(src_ascii.get());

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->initContainers(0);
    SRuntimeDb::get()->print();

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
    rdb()->addSource(src_root.get());

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->initContainers(0);

    //     auto rdb()->getContainer();
    auto pCalibratorPar =
        dynamic_cast<SCalContainer<6>*>(SRuntimeDb::get()->getCalContainer("FibersCalibratorPar"));

    ASSERT_NE(pCalibratorPar, nullptr);
}
