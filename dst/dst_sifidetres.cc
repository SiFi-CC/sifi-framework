// SiFi-Analysis framework includes
#include "SCategory.h" // for SCategory, SCategory::CatFibersCal
#include "SDRSource.h"
#include "SDatabase.h"
#include "SDetectorManager.h"
#include "SFibersDetector.h"
#include "SFibersLookup.h"
#include "SLookup.h" // for SLookupTable
#include "SParAsciiSource.h"
#include "SSiFiCCDetResImporter.h"
#include "STaskManager.h"
#include "SiFi.h"

// root includes
#include <TTree.h>

// STL includes
#include <bits/getopt_core.h> // for optarg, optind
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for atoi
#include <iostream>
#include <memory>
#include <string>

#include <getopt.h>

int main(int argc, char** argv)
{
    int events = 1000000;
    int save_samples = 0;

    std::string output("test.root");
    std::string params_file("params.txt");

    SDatabase db;
    SRuntimeDb::init(&db);

    sifi()->setSimulation(true);

    while (1)
    {
        static struct option long_options[] = {{"ss", no_argument, &save_samples, 1},
                                               {"events", required_argument, 0, 'e'},
                                               {"output", required_argument, 0, 'o'},
                                               {"params_file", required_argument, 0, 'p'},
                                               {0, 0, 0, 0}};

        int option_index = 0;

        int c = getopt_long(argc, argv, "e:o:p:", long_options, &option_index);

        if (c == -1) { break; }

        switch (c)
        {
            case 'e':
                events = atoi(optarg);
                break;
            case 'o':
                output = optarg;
                break;
            case 'p':
                params_file = optarg;
                break;
            default:
                break;
        }
    }

    SSiFiCCDetResImporter* unp = new SSiFiCCDetResImporter;

    SDRSource* source1 = new SDRSource();
    source1->addUnpacker(unp, {0x1000});

    while (optind < argc)
    {
        source1->addInput(argv[optind]);
        ++optind;
    }
    sifi()->addSource(source1);

    // output files
    sifi()->setOutputFileName(output);
    sifi()->book();

    // how many events to proceed
    int ev_limit = 0;
    if (events < 0) { ev_limit = source1->getEntries(); }
    else
        ev_limit = events < source1->getEntries() ? events : source1->getEntries();
    std::cout << source1->getEntries() << " events, analyze " << ev_limit << std::endl;

    // initialize parameters
    SRuntimeDb::get()->addSource(new SParAsciiSource(params_file));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    detm->initCategories();
    sifi()->buildCategory(SCategory::CatFibersCal, true);

    detm->initParameterContainers();
    detm->initTasks();

    SRuntimeDb::get()->addContainer(
        "FibersDDLookupTable",
        []() { return new SFibersLookupTable("FibersDDLookupTable", 0x1000, 0x1fff, 32); });

    // initialize tasks
    STaskManager* tm = STaskManager::instance();
    tm->initTasks();

    sifi()->setTree(new TTree());
    sifi()->loop(ev_limit);

    tm->finalizeTasks();

    sifi()->save();

    //     pm()->setParamDest("p.txt");
    //     pm()->writeDestination();

    return 0;
}
