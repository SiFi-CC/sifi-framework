// STL includes
#include <vector>
#include <time.h>
#include <string.h>
#include <list>
#include <iostream>
#include <chrono>
#include <algorithm>

#include <getopt.h>

// root includes
#include <TDatabasePDG.h>
#include <TVector3.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TH1.h>

// SiFi-Analysis framework includes
#include "SGeantTrack.h"
#include "SGeantFibersRaw.h"

#include "SiFi.h"
#include "SDetectorManager.h"
#include "STaskManager.h"
#include "SParManager.h"

#include "SFileSource.h"

#include "SFibersStackDetector.h"
#include "SFibersStackDDUnpacker.h"

#include "SProgressBar.h"

int main(int argc, char** argv)
{
    int events = 10000;
    int c;
    while(1)
    {
        static struct option long_options[] = {
            { "events", required_argument, 0, 'e' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;

        c = getopt_long(argc, argv, "e:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
            case 'e':
                events = atoi(optarg);
                break;
            default:
                break;
        }
    }

//     for (int i = 0; i < digi_status.size(); ++i)
//     {
//         std::cout << "Digitizer for " << digi_status[i].first << " with status " << digi_status[i].second << std::endl;
// //         std::cout << "Output file is " << oname.Data() << std::endl;
//     }

//     TString oname = file;
// 
//     if (oname.EndsWith(".root"))
//         oname.ReplaceAll(".root", "_digi.root");
//     else
//         oname.Append("_digi.root");

    // Init data manager
    SiFi * dataManager = SiFi::instance();
//     dataManager->setSimulation(true);

    std::vector< std::pair<std::string, int>> digi_status;
    while (optind < argc)
    {
//         std::cout << "Digitize " << argv[optind] << std::endl;
//         int status = simdst(argv[optind], events);
//         std::string f = argv[optind];
//         digi_status.push_back({f, status});

        std::string inpstr(argv[optind]);
        int n = std::count(inpstr.begin(), inpstr.end(), ':');
        if (n == 0)
        {
        }
        else if (n == 2)
        {
            int pos1 = inpstr.find(':');
            int pos2 = inpstr.find(':', pos1+1);

            std::string saddr = inpstr.substr(0, pos1);
            std::string type = inpstr.substr(pos1+1, pos2-pos1-1);
            std::string name = inpstr.substr(pos2+1, inpstr.length() - pos2-1);

            int n2 = std::count(saddr.begin(), saddr.end(), '-');
            uint16_t addr;
            uint8_t channel;
            if (n >= 1)
            {
                int p3 = saddr.find('-');
                addr = std::stoi(saddr.substr(0, p3), nullptr, 16);
                if (n == 1)
                channel = std::stoi(saddr.substr(p3+1, saddr.length() - p3));
            }
            else
            {
                addr = std::stoi(saddr, nullptr, 16);
                channel = 0;
            }

            SFileSource * source = new SFileSource();
            SFibersStackDDUnpacker * unp = new SFibersStackDDUnpacker(addr, channel);
            unp->setDataLen(1024);
            source->addUnpacker(unp, addr);
            source->setInput(name, 1024 * sizeof(float));
            sifi()->addSource(source);
        }

        ++optind;
    }

    // input files
//     dataManager->setInputFileName(file);
//     dataManager->open();

    // output files
    dataManager->setOutputFileName("test.root");
    dataManager->book();

    // how many events to proceed
    int ev_limit = 0;
    if (events < 0)
        ev_limit = dataManager->getEntries();
    else
        ev_limit = events < dataManager->getEntries() ? events : dataManager->getEntries();
    std::cout << dataManager->getEntries() << " events, analyze " << ev_limit << std::endl;

    dataManager->getCategory(SCategory::CatGeantTrack, true);

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

    dataManager->loop(/*ev_limit*/events);

    tm->finalizeTasks();

    dataManager->save();

    return 0;
}
