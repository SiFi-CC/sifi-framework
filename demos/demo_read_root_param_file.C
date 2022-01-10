#include <SCalContainer.h>
#include <SDatabase.h>
#include <SDetectorManager.h>
#include <SFibersDetector.h>
#include <SFibersLookup.h>
#include <SParDatabaseSource.h>
#include <SParRootSource.h>

void demo_read_root_param_file()
{
    SDatabase db;
    SRuntimeDb::init(&db);

    auto src_root = std::make_unique<SParRootSource>("params_root.root");

    rdb()->setExperiment("TEST");
    rdb()->addSource(src_root.get());

    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->setContainerMode("FibersClusterFinderPar", SDatabase::ContainerMode::None);
    SRuntimeDb::get()->setContainerMode("FibersDDCalibratorPar", SDatabase::ContainerMode::None);
    SRuntimeDb::get()->setContainerMode("FibersDDUnpackerPar", SDatabase::ContainerMode::None);
    SRuntimeDb::get()->setContainerMode("FibersGeomPar", SDatabase::ContainerMode::None);
    SRuntimeDb::get()->setContainerMode("FibersHitFinderFiberPar", SDatabase::ContainerMode::None);
    SRuntimeDb::get()->setContainerMode("FibersHitFinderPar", SDatabase::ContainerMode::None);

    rdb()->initContainers();

    rdb()->print();

    src_root->print();
}
