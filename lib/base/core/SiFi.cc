// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SiFi.h"
#include "SCategory.h"
#include "STaskManager.h"
#include "SProgressBar.h"

#include <TString.h>

#include <iostream>

/**
 * \class SiFi
\ingroup lib_base

Access point to load, read and write data.

The main class of the SiFi Framework. The SiFi is responsible for
managing all data operations. It loads a root tree from specified file.
*/

SCategory * gNullSCategoryPtr = 0;

SiFi * SiFi::mm = nullptr;
SiFi::CategoryInfo SiFi::cinfovec[SCategory::CatLimitDoNotUse * 2] = {};

/**
 * Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SiFi * SiFi::instance()
{
    if (!mm)
        mm = new SiFi;

    return mm;
}

/**
 * Shortcut
 *
 * \return SiFi instance
 */
SiFi * sifi()
{
    return SiFi::instance();
}

/**
 * Default constructor
 */
SiFi::SiFi()
    : outputFile(nullptr), outputFileName("output.root")
    , outputTree(nullptr), outputTreeName("S")
    , inputTree(nullptr), inputTreeName("S")
    , numberOfEntries(-1), currentEntry(-1), event(nullptr)
    , sim(false), branches_set(false)
{
}

SiFi::~SiFi()
{
    if (outputFile)
        delete outputFile;

    if (inputTree)
        delete inputTree;

    for (auto & c : categories)
        delete c.second;

    for (auto & s : inputSources)
        delete s;
}


/**
 * Set simulation run.
 *
 * \param simulation is simulation run
 */
void SiFi::setSimulation(bool simulation)
{
    sim = simulation;
    if (sim)
    {
        // Here register all detector independen categories
        size_t sizes[1];
        sizes[0] = 200;
        registerCategory(SCategory::CatGeantTrack, "MGeantTrack", 1, sizes, true);
    }
}

/**
 * Creates a new file and an empty root tree with output categories.
 *
 * \param with_tree create output tree
 * \return true if success
 */
bool SiFi::book(bool with_tree)
{
    // Create file and open it
    outputFile = new TFile(outputFileName.c_str(), "RECREATE");

    if (!with_tree) return true;

    if (!outputFile->IsOpen())
    {
        std::cerr  << "[Error] in SiFi: could not create " <<
        outputFileName.c_str() << std::endl;
        return false;
    }

    // Create tree
    outputTree = new TTree(outputTreeName.c_str(), outputTreeName.c_str());

    return true;
}

/**
 * Writes the tree to file and close it.
 *
 * \return success
 */
bool SiFi::save()
{
    if (outputFile)
    {
        outputFile->cd();
        if (outputTree)
            outputTree->Write();        // Writing the tree to the file
        fileHeader.Write("FileHeader");

        if (event)
            event->Write("Event");

        outputFile->Close();        // and closing the tree (and the file)
        return true;
    }

    return false;
}

/**
 * Fills the current event into the tree.
 *
 * \return status of Fill() method
 */
Int_t SiFi::fill()
{
    if (!branches_set)
        initBranches();

    // clear the current event data
    for (auto & element : categories)
        element.second->compress();

    if (!outputTree)
        return -1;

    // fill tree
    Int_t status = outputTree->Fill();

    return status;
}

/**
 * Opens a file and loads the root tree.
 *
 * \return success
 */
bool SiFi::open()
{
    if (inputTree)
        delete inputTree;

    inputTree = new TChain(inputTreeName.c_str());

    if (inputTree == 0)
    {
        std::cerr << "[Error] in SiFi: cannot open ROOT file" << "\n";
        return false;
    }

    for (auto & s : inputSources)
    {
//         printf("Add file %s\n", inputFiles[i].c_str());
//         inputTree->Add(inputFiles[i].c_str());
        s->open();
    }

    inputTree->SetBranchStatus("*");

    numberOfEntries = inputTree->GetEntriesFast();

    return true;
}

/**
 * Register category.
 *
 * Every category must be registered first before using it (reading or writing).
 * The detector related categories should be registerd inside
 * MDetector::initCategories() method.
 *
 * \param cat category ID
 * \param name category name
 * \param dim address dimension
 * \param sizes dimension sizes
 * \param simulation simulation run
 * \return success
 */
bool SiFi::registerCategory(SCategory::Cat cat, const std::string & name, size_t dim, size_t * sizes, bool simulation)
{
    int pos = getCategoryIndex(cat, simulation);

    if (SiFi::cinfovec[pos].registered == true)
        return true;

    CategoryInfo & cinfo = SiFi::cinfovec[pos];
    cinfo.registered = true;
    cinfo.cat = cat;
    cinfo.name = name;
    cinfo.simulation = simulation;
    cinfo.dim = dim;
    for (int i = 0; i < dim; ++i)
        cinfo.sizes[i] = sizes[i];
    cinfo.ptr = nullptr;

    return true;
}

/**
 * Register linear category of length n.
 *
 * Every category must be registered first before using it (reading or writing).
 * The detector related categories should be registerd inside
 * MDetector::initCategories() method.
 *
 * \param cat category ID
 * \param name category name
 * \param n linear length
 * \param simulation simulation run
 * \return success
 */
bool SiFi::registerCategory(SCategory::Cat cat, const std::string & name, size_t n, bool simulation)
{
    int pos = getCategoryIndex(cat, simulation);

    if (SiFi::cinfovec[pos].registered == true)
        return true;

    CategoryInfo & cinfo = SiFi::cinfovec[pos];
    cinfo.registered = true;
    cinfo.cat = cat;
    cinfo.name = name;
    cinfo.simulation = simulation;
    cinfo.dim = 1;
    cinfo.sizes[0] = n;
    cinfo.ptr = nullptr;

    return true;
}

/**
 * Init all braches in the output tree.
 *
 * Uses registered categories info to init branches. If the category is
 * persistent, will be written to output tree. The persistent input category
 * will be rewritten to output tree.
 */
void SiFi::initBranches()
{
    size_t limit = SCategory::CatLimitDoNotUse * 2;
    for (size_t i = 0; i < limit; ++i)
    {
        CategoryInfo & cinfo = SiFi::cinfovec[i];
        if (!cinfo.persistent)
            continue;

        outputTree->Branch(cinfo.ptr->getName(), cinfo.ptr, 16000, 2);
    }

    branches_set = true;
}

/**
 * Build category based on its ID. Category must be first registered.
 *
 * \param cat category ID
 * \param persistent set category persistent
 * \return pointer to category object
 */
SCategory * SiFi::buildCategory(SCategory::Cat cat, bool persistent)
{
    if (!outputTree)
        return gNullSCategoryPtr;

    if (categories[cat])
        return categories[cat];

    int pos = getCategoryIndex(cat, sim);

    CategoryInfo & cinfo = SiFi::cinfovec[pos];
    if (cinfo.registered == false)
        return gNullSCategoryPtr;

    cinfo.persistent = persistent;
    SCategory * cat_ptr = new SCategory(cinfo.name.c_str(), cinfo.dim, cinfo.sizes, cinfo.simulation);
    cinfo.ptr = cat_ptr;

    if (cat_ptr)
    {
        categories[cat] = cat_ptr;
        fileHeader.catName[cat] = cinfo.name;
        return cat_ptr;
    }

    return gNullSCategoryPtr;
}

/**
 * Get the category.
 *
 * If category is not open, opens the category from input tree.
 *
 * \param cat category ID
 * \param persistent set category persistent
 * \return pointer to category object
 */
SCategory * SiFi::getCategory(SCategory::Cat cat, bool /*persistent*/)
{
    int pos = getCategoryIndex(cat, sim);
    CategoryInfo & cinfo = SiFi::cinfovec[pos];
    if (cinfo.registered == false)
        return gNullSCategoryPtr;
    if (cinfo.ptr)
        return cinfo.ptr;
    // TODO do wee need to optn category if not exists?
//     SCategory * c = 0;//openCategory(cat, persistent);
//     if (c)
//         return c;

    return gNullSCategoryPtr;
}

/**
 * Reads entry of the current tree.
 *
 * The categories are filled with the data saved in tree entry at index \p i.
 *
 * \param i event number
 */
void SiFi::getEntry(int i)
{
    if (!inputTree)
    {
        std::cerr << "[Warning] in SiFi: no input tree is opened. Cannot get any entry." << "\n";
        return;
    }

    if (i < numberOfEntries)
    {
        currentEntry = i;
        inputTree->GetEntry(i);
    }
}

/**
 * Returns number of entries in the current tree.
 *
 * \return number of entries
 */
Long64_t SiFi::getEntries()
{
    if (!inputTree)
    {
        std::cerr << "[Warning] in SiFi: no input tree is opened. Cannot get any entry." << "\n";
        return -1;
    }
    numberOfEntries = inputTree->GetEntries();
    return numberOfEntries;
}

/**
 * Print info about the categories.
 */
void SiFi::print() const
{
    outputFile->cd();
    printf("There are %zu categories in the output tree\n", categories.size());
    for (auto & element : categories)
        element.second->print();
}

/**
 * Clear all categories.
 */
void SiFi::clear()
{
    for (auto & element : categories)
        element.second->clear();
}

/**
 * Loop ever entries.
 *
 * \param entries number to entries to loop over
 * \param show_progress_bar display progress bar
 */
void SiFi::loop(long entries, bool show_progress_bar)
{
    for (auto & s : inputSources)
    {
        bool rc = s->open();
        if (!rc) {
            printf("Could not open source %d\n", 0);    // TODO add some feedback info
            abort();
        }
    }

    STaskManager * tm = STaskManager::instance();

    SProgressBar pb(entries);

    // go over all events
    ulong i;
    for (i = 0 ; i < entries; ++i)
    {
        clear();
        bool flag = false;

        for (auto & s : inputSources)
        {
            flag = s->readCurrentEvent();
            if (!flag) break;
        }

        if (!flag) break;

        if (show_progress_bar)
            ++pb;

        getEntry(i);
        tm->runTasks();
        fill();
    }
    pb.finish();
    printf("*** SiFi finished after %lu events\n", i);
}

/**
 * Set event TODO what was that?
 *
 * \param e event
 */
void SiFi::setEvent(SEvent* e)
{
    if (event)
        delete event;
    event = e;
}
