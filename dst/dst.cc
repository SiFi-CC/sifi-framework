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

#include "SDDSource.h"

#include "SFibersStackDDUnpacker.h"
#include "SFibersStackDetector.h"
#include "SFibersStackLookup.h"

#include "SProgressBar.h"

int main(int argc, char** argv)
{
    int events = 1000000;
    int save_samples = 0;

    std::string output("test.root");
    std::string params_file("params.txt");

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

    SFibersStackDDUnpacker* unp = new SFibersStackDDUnpacker();
    SFibersStackDDUnpacker::saveSamples(save_samples);

    while (optind < argc)
    {
        std::string inpstr(argv[optind]);
        int n = std::count(inpstr.begin(), inpstr.end(), ':');

        if (n == 2)
        {
            int pos1 = inpstr.find(':');
            int pos2 = inpstr.find(':', pos1 + 1);

            std::string saddr = inpstr.substr(0, pos1);
            std::string type = inpstr.substr(pos1 + 1, pos2 - pos1 - 1);
            std::string name = inpstr.substr(pos2 + 1, inpstr.length() - pos2 - 1);

            uint16_t addr = std::stoi(saddr, nullptr, 16);

            SDDSource* source = new SDDSource(addr);
            unp->setDataLen(1024);
            source->addUnpacker(unp, {addr});
            source->setInput(name, 1024 * sizeof(float));
            sifi()->addSource(source);
        }

        ++optind;
    }

    // output files
    sifi()->setOutputFileName(output);
    sifi()->book();

    // how many events to proceed
    int ev_limit = 0;
    if (events < 0) { ev_limit = sifi()->getEntries(); }
    else
        ev_limit = events < sifi()->getEntries() ? events : sifi()->getEntries();
    std::cout << sifi()->getEntries() << " events, analyze " << ev_limit << std::endl;

    sifi()->getCategory(SCategory::CatGeantTrack, true);

    // initialize parameters
    pm()->setParamSource(params_file);
    pm()->parseSource();

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersStackDetector("FibersStack"));

    detm->initTasks();
    detm->initParameterContainers();
    detm->initCategories();

    pm()->addLookupContainer(
        "SFibersStackDDLookupTable",
        new SFibersStackLookupTable("SFibersStackDDLookupTable", 0x1000, 0x1fff, 32));

    // initialize tasks
    STaskManager* tm = STaskManager::instance();
    tm->initTasks();

    sifi()->setTree(new TTree());
    sifi()->loop(/*ev_limit*/ events);

    tm->finalizeTasks();

    sifi()->save();

    //     pm()->setParamDest("p.txt");
    //     pm()->writeDestination();

    return 0;
}
