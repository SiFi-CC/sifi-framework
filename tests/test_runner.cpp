#define CPPUNIT_HAVE_NAMESPACES 1

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <getopt.h>

#include <iostream>
#include <stdexcept>

using namespace CppUnit;
using namespace std;

int main(int argc, char* argv[])
{
    bool show_help = false;
    while (1)
    {
        static struct option long_options[] = {/* These options set a flag. */

                                               {0, 0, 0, 0}};
        /* getopt_long stores the option index here. */
        int option_index = 0;

        int c = getopt_long(argc, argv, "h", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0) break;
                printf("option %s", long_options[option_index].name);
                if (optarg) printf(" with arg %s", optarg);
                printf("\n");
                break;

            case 'h':
            case '?':
                /* getopt_long already printed an error message. */
                show_help = true;
                break;

            default:
                abort();
        }
    }

    // Create the event manager and test controller
    CppUnit::TestResult controller;

    // Add a listener that colllects test result
    CppUnit::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that print dots as test run.
    CppUnit::TextTestProgressListener progress;
    controller.addListener(&progress);

    TestRunner runner;
    TestFactoryRegistry& registry = TestFactoryRegistry::getRegistry();

    // run all tests if none specified on command line
    Test* test_to_run = registry.makeTest();
    if (show_help)
    {
        int n = test_to_run->countTestCases();
        printf("Avalable tests\n");
        for (int i = 0; i < n-1; ++i)
        {
            auto* t = test_to_run->getChildTestAt(i);
            printf(" * [%d/%d] %s\n", i+1, n-1, t->getName().c_str());
        }
        std::exit(EXIT_SUCCESS);
    }

    if (optind < argc)
    {
        while (optind < argc) {
            auto * t = test_to_run->findTest(argv[optind++]);
            runner.addTest(t);
        }
    }
    else
    {
        runner.addTest(test_to_run);
    }

    try
    {
        std::cout << "Running tests" /*<<  testPath*/ << endl;
        ;
        runner.run(controller);

        // Print test in a compiler compatible format.
        CppUnit::CompilerOutputter outputter(&result, std::cerr);
        outputter.write();
    }
    catch (std::invalid_argument& e) // Test path not resolved
    {
        std::cerr << std::endl << "ERROR: " << e.what() << std::endl;
        return 0;
    }

    return result.wasSuccessful() ? 0 : 1;
}
