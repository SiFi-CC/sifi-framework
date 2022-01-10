#include <SCalContainer.h>
#include <SDatabase.h>
#include <SDetectorManager.h>
#include <SFibersLookup.h>
#include <SFibersDetector.h>
#include <SParRootSource.h>

void demo_create_root_param_file()
{
    SDatabase db;
    SRuntimeDb::init(&db);

    auto src_root = std::make_unique<SParRootSource>("params_root.root");
    src_root->setOpenMode(SourceOpenMode::Output);

    SRuntimeDb::get()->setTarget(src_root.get());
    //     SRuntimeDb::get()->addSource(src_root.get());

    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->setContainerMode("FibersCalibratorPar", SDatabase::ContainerMode::Create);

    SRuntimeDb::get()->initContainers(0);

    //     SCalContainer<6>* pCalibratorPar = nullptr;
    auto pCalibratorPar =
        dynamic_cast<SCalContainer<6>*>(SRuntimeDb::get()->getCalContainer("FibersCalibratorPar"));

    if (pCalibratorPar)
    {
        SFibersChannel chan_l;
        chan_l.m = 0;
        chan_l.l = 0;
        chan_l.s = 0;
        chan_l.side = 'l';
        SFibersChannel chan_r = chan_l;
        chan_r.side = 'r';

        SCalPar<6> def;
        pCalibratorPar->setDefault(&def);

        auto _cp_l = pCalibratorPar->getPar(&chan_l);
        auto _cp_r = pCalibratorPar->getPar(&chan_r);

        (*_cp_l)[0] = 13;
        pCalibratorPar->print();
    }
    else
    {
        printf("No container.\n");
    }

    SRuntimeDb::get()->writeTarget();
}
