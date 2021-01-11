// STL includes
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <string.h>
#include <time.h>
#include <vector>

#include <getopt.h>

// root includes
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>

// SiFi-Analysis framework includes
#include "SGeantFibersRaw.h"
#include "SGeantTrack.h"

#include "SDetectorManager.h"
#include "SParManager.h"
#include "STaskManager.h"
#include "SiFi.h"

#include "SDRSource.h"

#include "SFibersStackDDUnpacker.h"
#include "SFibersStackDetector.h"
#include "SFibersStackLookup.h"
#include "SSiFiCCDetResImporter.h"

#include "SProgressBar.h"

int main(int argc, char** argv)
{
    int events = 1000000;
    int save_samples = 0;

    std::string output("test.root");
    std::string params_file("params.txt");

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
    pm()->setParamSource(params_file);
    pm()->parseSource();

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersStackDetector("FibersStack"));

    detm->initCategories();
    sifi()->buildCategory(SCategory::CatFibersStackCal, true);

    detm->initParameterContainers();
    detm->initTasks();

    pm()->addLookupContainer(
        "SFibersStackDDLookupTable",
        new SFibersStackLookupTable("SFibersStackDDLookupTable", 0x1000, 0x1fff, 32));

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
