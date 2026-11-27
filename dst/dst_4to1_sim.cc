// SiFi-Analysis framework includes
#include "SCategory.h" // for SCategory, SCategory::CatFibersCal
#include "SDatabase.h"
#include "SDetectorManager.h"
#include "SFibersDetector.h"
#include "SFibersLookup.h"
#include "SMultiFibersLookup.h"
#include "SLookup.h" // for SLookupTable
#include "SParAsciiSource.h"
#include "_SSiFiCCDetResImporter.h"
#include "STaskManager.h"
#include "SiFi.h"
#include "SSimSource.h"
#include "SSimImporter.h"

// root includes
#include <TTree.h>

// STL includes
#include <bits/getopt_core.h> // for optarg, optind
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for atoi
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <getopt.h>

int main(int argc, char** argv)
{
    int events = -1;
    int save_samples = 0;
    int no_tasks = 0;

    std::string output("test.root");
    std::string params_file("params.txt");

    sifi()->setSimulation(true);

    while (1)
    {
        static struct option long_options[] = {{"ss", no_argument, &save_samples, 1},
                                               {"no_tasks", no_argument, &no_tasks, 1},
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

    if (no_tasks)
    {
        // In no-tasks mode we want to be able to run minimal I/O even if some
        // parameter containers are not present in the params file. The framework
        // asserts on missing containers by default, so relax that here.
        sifi()->disableAssertations();
    }

    SSimImporter* unp = new SSimImporter;

    SSimSource* source1 = new SSimSource();
    source1->addUnpacker(unp, {0x0000});

    while (optind < argc)
    {
        source1->addInput(argv[optind]);
        ++optind;
    }
    sifi()->addSource(source1);

    {
        // Fail early with a clear message instead of letting the parameter source
        // fail later with a less obvious error.
        std::ifstream params_stream(params_file);
        if (!params_stream.good())
        {
            std::cerr << "Source file " << params_file << " could not be opened!" << std::endl;
            return 1;
        }
    }

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
    pm()->addSource(new SParAsciiSource(params_file));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers", 1, 7, 55));
    // --no_tasks skips task and parameter-container initialisation so the program
    // can be used purely to convert simulation input into a SiFi tree. Categories
    // are still initialised because the importer writes into them.
    if (!no_tasks)
    {
        detm->initTasks();
        detm->initParameterContainers();
    }
    detm->initCategories();
//     sifi()->buildCategory(SCategory::CatFibersCal, true);


//     detm->initTasks(); //creates SFibersCluster and SFibersHitSim

//     pm()->addLookupContainer("TPLookupTable", std::make_unique<SSiPMsLookupTable>("TPLookupTable", 0x1000, 0x1fff, 20000));
    pm()->addLookupContainer("FibersDDLookupTable", std::make_unique<SFibersLookupTable>("FibersDDLookupTable", 0x1000, 0x1fff, 32));
    pm()->addLookupContainer("4to1SiPMtoFibersLookupTable", std::make_unique<SMultiFibersLookupTable>("4to1SiPMtoFibersLookupTable", 0x1000, 0x1fff, 20000));
    
    // initialize tasks
    STaskManager* tm = STaskManager::instance();
    if (!no_tasks) tm->initTasks();

    sifi()->setTree(new TTree());

    sifi()->loop(ev_limit);

    if (!no_tasks) tm->finalizeTasks();

    sifi()->save();

    //     pm()->setParamDest("p.txt");
    //     pm()->writeDestination();

    return 0;
}
