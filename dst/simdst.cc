// STL includes
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

#include "SFibersStackDetector.h"

#include "SProgressBar.h"

/*
int simdst(const std::string & file, int events = 1000)
{
    TString oname = file;

    if (oname.EndsWith(".root"))
        oname.ReplaceAll(".root", "_digi.root");
    else
        oname.Append("_digi.root");

    // Init data manager
    SiFiManager * dataManager = SiFiManager::instance();
    dataManager->setSimulation(true);

    // input files
//     dataManager->setInputFileName(file);
    dataManager->open();

    // output files
    dataManager->setOutputFileName(oname.Data());
    dataManager->book();

    // how many events to proceed
    int ev_limit = 0;
    if (events < 0)
        ev_limit = dataManager->getEntries();
    else
        ev_limit = events < dataManager->getEntries() ? events : dataManager->getEntries();
    std::cout << dataManager->getEntries() << " events, analyze " << ev_limit << std::endl;

    dataManager->openCategory(SCategory::CatGeantTrack, true);

    // initialize parameters
    SParManager * pm = SParManager::instance();
    pm->setParamSource("params.txt");
    pm->parseSource();

    // initialize detectors
    SDetectorManager * detm = SDetectorManager::instance();

    detm->addDetector(new SFibersStackDetector("FibersStack"));

    detm->initTasks();
    detm->initParameterContainers();
    detm->initCategories();

    // initialize tasks
    STaskManager * tm = STaskManager::instance();
    tm->initTasks();

    SProgressBar pb(ev_limit);

    // go over all events
    for (int i=0 ; i < ev_limit; ++i)
    {
        ++pb;
        dataManager->clear();

        dataManager->getEntry(i);

        tm->runTasks();

        dataManager->fill();
    }

    tm->finalizeTasks();

    dataManager->save();

    return 0;
}
*/
int main(int argc, char** argv)
{
    int events = 10000;

    while (1)
    {
        static struct option long_options[] = {{"events", required_argument, 0, 'e'}, {0, 0, 0, 0}};

        int option_index = 0;

        int c = getopt_long(argc, argv, "e:", long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
            case 'e':
                events = atoi(optarg);
                break;
            default:
                break;
        }
    }

    std::vector<std::pair<std::string, int>> digi_status;
    while (optind < argc)
    {
        std::cout << "Digitize " << argv[optind] << std::endl;
        //         int status = simdst(argv[optind], events);
        std::string f = argv[optind];
        //         digi_status.push_back({f, status});
        ++optind;
    }

    for (int i = 0; i < digi_status.size(); ++i)
    {
        std::cout << "Digitizer for " << digi_status[i].first << " with status "
                  << digi_status[i].second << std::endl;
        //         std::cout << "Output file is " << oname.Data() << std::endl;
    }

    return 0;
}
