// SiFi-Analysis framework includes
#include "SCBSource.h"
#include "SCategory.h" // for SCategory, SCategory::CatGeantTrack
#include "SDDSource.h"
#include "SDatabase.h"
#include "SDetectorManager.h"
#include "SFibersCBUnpacker.h"
#include "SFibersDDUnpacker.h"
#include "SFibersDetector.h"
#include "SFibersHLDUnpacker.h"
#include "SFibersLookup.h"
#include "SFibersPMIUnpacker.h"
#include "SFibersPetirocUnpacker.h"
#include "SFibersTPUnpacker.h"
#include "SFibersTTreeUnpacker.h"
#include "SHLDSource.h"
#include "SKSSource.h"
#include "SLookup.h"
#include "SPMISource.h"
#include "SParAsciiSource.h"
#include "SPetirocSource.h"
#include "STPSource.h"
#include "STTreeSource.h"
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

    SDatabase db;
    SRuntimeDb::init(&db);

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
            // std::string ext = name.substr(name.size() - 4, name.size() - 1);
            std::string ext = name.substr(name.find_last_of(".") + 1);
            uint16_t addr = std::stoi(saddr, nullptr, 16);

            if (ext == "dat")
            {
                SFibersDDUnpacker* unp = new SFibersDDUnpacker();
                SFibersDDUnpacker::saveSamples(save_samples);

                SDDSource* source = new SDDSource(addr);
                unp->setDataLen(1024);
                source->addUnpacker(unp, {addr});
                source->setInput(name, 1024 * sizeof(float));
                sifi()->addSource(source);
            }
            else if (ext == "csv")
            {
                SFibersDDUnpacker* unp = new SFibersDDUnpacker();
                SFibersDDUnpacker::saveSamples(save_samples);

                SKSSource* source = new SKSSource(addr);
                source->addUnpacker(unp, {addr});
                source->setInput(name);
                sifi()->addSource(source);
            }
            /*
             * technically the output file from the Petiroc2A board is in the CSV format with a
             * space delimiter but we use the .roc extension since the .csv extension has been used
             * for the oscilloscope csv output
             */
            else if (ext == "roc")
            {
                //./sifi_dst 0x1000::data_44358_8859100231.roc -e 100000 -p params.txt -o
                // sifi_results.root
                SFibersPetirocUnpacker* unp = new SFibersPetirocUnpacker();
                SPetirocSource* source = new SPetirocSource(addr);
                source->addUnpacker(unp, {addr});
                source->setInput(name);
                sifi()->addSource(source);
            }
            else if (ext == "pmi")
            {
                SFibersPMIUnpacker* unp = new SFibersPMIUnpacker();

                SPMISource* source = new SPMISource(addr);
                source->addUnpacker(unp, {addr});
                source->setInput(name);
                sifi()->addSource(source);
            }
            else if (ext == ".txt")
            {
                SFibersCBUnpacker* unp = new SFibersCBUnpacker();

                SCBSource* source = new SCBSource(addr);
                source->addUnpacker(unp, {addr});
                source->setInput(name);
                sifi()->addSource(source);
            }
            else if (ext == "hld")
            {
                SFibersHLDUnpacker* unp = new SFibersHLDUnpacker();

                SHLDSource* source = new SHLDSource(addr);
                source->addUnpacker(unp, {addr});
                source->setInput(name);
                sifi()->addSource(source);
            }
            else if (ext == "root")
            {
                TFile* input_file;
                input_file = new TFile(name.c_str());
                if (!input_file->IsOpen())
                {
                    std::cerr << "##### Error in dst.cc Could not open input .root file!"
                              << std::endl;
                    std::cerr << name << std::endl;
                }

                // file close
                if ((TTree*)input_file->Get("data"))
                {
                    SFibersTPUnpacker* unp = new SFibersTPUnpacker();
                    STPSource* source = new STPSource(addr);
                    source->addUnpacker(unp, {addr});
                    source->setInput(name);
                    sifi()->addSource(source);
                }
                else if ((TTree*)input_file->Get("FiberCoincidences"))
                {
                    SFibersTTreeUnpacker* unp = new SFibersTTreeUnpacker();
                    STTreeSource* source = new STTreeSource(addr);
                    source->addUnpacker(unp, {addr});
                    source->setInput(name);
                    sifi()->addSource(source);
                }
                else
                {
                    std::cerr << "##### Error in dst: unknown tree name in the input .root file!"
                              << std::endl;
                    std::cerr << "Acceptable tree names: data, ..." << std::endl;
                }
            }
            else
            {
                std::cerr << "##### Error in dst: unknown data file extension!" << std::endl;
                std::cerr << "Acceptable extensions: *.dat, *.csv, *.pmi, *.txt and .root"
                          << std::endl;
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
    SRuntimeDb::get()->addSource(SIFI::make_ascii_source(params_file));

    // initialize detectors
    SDetectorManager* detm = SDetectorManager::instance();

    detm->addDetector(new SFibersDetector("Fibers"));

    SRuntimeDb::get()->addContainer(
        "FibersDDLookupTable",
        []() { return new SFibersLookupTable("FibersDDLookupTable", 0x1000, 0x1fff, 32); });
    SRuntimeDb::get()->addContainer(
        "FibersTPLookupTable",
        []() { return new SFibersLookupTable("FibersTPLookupTable", 0x1000, 0x1fff, 5000); });
    SRuntimeDb::get()->addContainer(
        "FibersPMILookupTable",
        []() { return new SFibersLookupTable("FibersPMILookupTable", 0x1000, 0x1fff, 64); });
    SRuntimeDb::get()->addContainer(
        "FibersPETIROCLookupTable",
        []() { return new SFibersLookupTable("FibersPETIROCLookupTable", 0x1000, 0x1fff, 64); });
    SRuntimeDb::get()->addContainer(
        "FibersTTreeLookupTable",
        []() { return new SFibersLookupTable("FibersTTreeLookupTable", 0x1000, 0x1fff, 64); });
    SRuntimeDb::get()->addContainer(
        "FibersHLDLookupTable",
        []() { return new SFibersLookupTable("FibersHLDLookupTable", 0x1000, 0x1fff, 64); });

    SRuntimeDb::get()->initContainers(0);

    // initialize tasks
    STaskManager* tm = STaskManager::instance();
    tm->initTasks();

    sifi()->setTree(new TTree());
    sifi()->loop(/*ev_limit*/ events);

    tm->finalizeTasks();

    sifi()->save();

    //     SRuntimeDb::get()->setParamDest("p.txt");
    //     SRuntimeDb::get()->writeDestination();

    return 0;
}
