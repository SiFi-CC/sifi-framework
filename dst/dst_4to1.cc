// SiFi-Analysis framework includes
#include "SCategory.h" // for SCategory, SCategory::CatGeantTrack
#include "SDDSource.h"
#include "SDatabase.h"
#include "SDetectorManager.h"
#include "SFibersDDUnpacker.h"
#include "SFibersDetector.h"
#include "SFibersLookup.h"
#include "SFibersTP4to1Unpacker.h"
#include "SLookup.h"
#include "STP4to1Source.h"
// #include "STPSource.h"
#include "STP4to1Extractor.h"
#include "SParAsciiSource.h"
#include "STaskManager.h"
#include "SiFi.h"

// root includes
#include <TTree.h>

// STL includes
#include <algorithm>
#include <bits/getopt_core.h> // for optarg, optind
#include <cstdint>            // for uint16_t
#include <cstdlib>            // for atoi, exit, EXIT_FAILURE
#include <iostream>
#include <memory> // for allocator, make_unique
#include <string> // for string, operator==, basic_string

#include <getopt.h>

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
            std::string ext = name.substr(name.find_last_of(".") + 1);
            uint16_t addr = std::stoi(saddr, nullptr, 16);
            std::cout << "addr" << addr << std::endl;
            if(ext == "root")
            {
                SFibersTP4to1Unpacker* unp = new SFibersTP4to1Unpacker();
                STP4to1Source* source= new STP4to1Source(addr);
                source->addUnpacker(unp, {addr});
                source->setInput(name);
                sifi()->addSource(source);
            }
            else
            {
                std::cerr << "##### Error in dst: unknown data file extension!" << std::endl;
                std::cerr << "Acceptable extensions: *.root" << std::endl;
                std::cerr << "Given data file: " << name << std::endl;
                std::exit(EXIT_FAILURE);
            }

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
    pm()->addSource(new SParAsciiSource(params_file));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers", 4, 4, 16));

    detm->initTasks();
    detm->initParameterContainers();
    detm->initCategories();

    pm()->addLookupContainer("FibersPMILookupTable", std::make_unique<SFibersLookupTable>("FibersPMILookupTable", 0x1000, 0x1fff, 20000));
    
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
