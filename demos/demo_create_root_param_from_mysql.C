#include <SCalContainer.h>
#include <SDatabase.h>
#include <SDetectorManager.h>
#include <SFibersDetector.h>
#include <SFibersLookup.h>
#include <SParDatabaseSource.h>
#include <SParRootSource.h>

void demo_create_root_param_from_mysql()
{
    SDatabase db;
    SRuntimeDb::init(&db);

    auto src_mysql = std::make_unique<SParDatabaseSource>();

    auto src_root = std::make_unique<SParRootSource>("params_root.root");

    rdb()->setExperiment("TEST");
    rdb()->addSource(src_mysql.get());
    rdb()->setTarget(src_root.get());
    //     rdb()->addSource(src_root.get());

    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    rdb()->setContainerMode("FibersGeomPar", SDatabase::ContainerMode::Forward);
    rdb()->setContainerMode("FibersCalibratorPar", SDatabase::ContainerMode::Forward);

    rdb()->initContainers();

    rdb()->print();

    rdb()->writeDestination();

    src_root->print();
}
