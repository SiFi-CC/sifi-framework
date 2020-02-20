// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SiFiManager.h"

#include "SCategory.h"

#include <iostream>

/** \class SiFiManager
\ingroup lib_base

Access point to load, read and write data.

The main class of the SiFi Framework. The SiFiManager is responsible for
managing all data operations. It loads a root tree from specified file.
*/

SCategory * gNullSCategoryPtr = 0;

SiFiManager * SiFiManager::mm = nullptr;

/** Returns instance of the Detector Manager class.
 *
 * \return manager instance
 */
SiFiManager * SiFiManager::instance()
{
    if (!mm)
        mm = new SiFiManager;

    return mm;
}

/** Shortcut
 * \return SiFiManager instance
 */
SiFiManager * mapt()
{
    return SiFiManager::instance();
}

/** Default constructor
 */
SiFiManager::SiFiManager() :
    outputFile(nullptr), outputTree(nullptr), outputTreeTitle("M"),
    outputTreeName("M"), outputFileName("output.root"),
    inputTree(nullptr), inputTreeTitle("M"), inputTreeName("M"),
    numberOfEntries(-1), currentEntry(-1), branches_set(false)
{
}

/** Set simulation run.
 *
 * \param simulation is simulation run
 */
void SiFiManager::setSimulation(bool simulation)
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

/** Set single input file.
 *\param file file name
 */
void SiFiManager::setInputFileName(const std::string& file)
{
    inputFiles.clear();
    inputFiles.push_back(file);
}

/** Add single input file to the list.
 * \param file file name
 */
void SiFiManager::addInputFileName(const std::string& file)
{
    inputFiles.push_back(file);
}

/** Set multiple input files to the list.
 * \param files file names
 */
void SiFiManager::setInputFileNames(const std::vector<std::string> & files)
{
    inputFiles = files;
}

/** Add multiple input files to the list.
 * \param files file names
 */
void SiFiManager::addInputFileNames(const std::vector<std::string> & files)
{
    inputFiles.insert(inputFiles.end(), files.begin(), files.end());
}

/** Creates a new file and an empty root tree with output categories.
 * \param with_tree create output tree
 * \return true if success
 */
bool SiFiManager::book(bool with_tree)
{
    // Create file and open it
    outputFile = new TFile(outputFileName.c_str(), "RECREATE");

    if (!with_tree) return true;

    if (!outputFile->IsOpen())
    {
        std::cerr  << "[Error] in SiFiManager: could not create " <<
        outputFileName.c_str() << std::endl;
        return false;
    }

    // Create tree
    outputTree = new TTree(outputTreeName.c_str(), outputTreeTitle.c_str());

    return true;
}

/** Writes the tree to file and close it.
 * \return success
 */
bool SiFiManager::save()
{
    if (outputFile)
    {
        outputFile->cd();
        if (outputTree)
            outputTree->Write();        // Writing the tree to the file
        outputFile->Close();        // and closing the tree (and the file)
        return true;
    }

    return false;
}

/** Fills the current event into the tree.
 * \return status of Fill() method
 */
Int_t SiFiManager::fill()
{
    if (!branches_set)
        initBranches();

    // clear the current event data
    for (CatMap::iterator it = categories.begin(); it != categories.end(); ++it)
    {
        it->second->compress();
    }

    // fill tree
    Int_t status = outputTree->Fill();
//     printf("Fill status = %d\n", status);

    return status;
}

/** Opens a file and loads the root tree.
 * \return success
 */
bool SiFiManager::open()
{
    if (inputTree)
        delete inputTree;

    inputTree = new TChain(inputTreeName.c_str());

    if (inputTree == 0)
    {
        std::cerr << "[Error] in SiFiManager: cannot open ROOT file" << "\n";
        return false;
    }

    for (uint i = 0; i < inputFiles.size(); ++i)
    {
        printf("Add file %s\n", inputFiles[i].c_str());
        inputTree->Add(inputFiles[i].c_str());
    }

    if (!inputTree) {
        std::cerr << "[Error] in SiFiManager open: openTree == NULL" << "\n";
        return false;
    }

    inputTree->SetBranchStatus("*");

    numberOfEntries = inputTree->GetEntriesFast();

    return true;
}

/** Register category.
 *
 * Every category must be registered first before using i (reading or writing).
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
bool SiFiManager::registerCategory(SCategory::Cat cat, std::string name, size_t dim, size_t * sizes, bool simulation)
{
    int pos = cat * (1+(int)simulation);

    if (cinfovec[pos].registered == true)
        return false;

    CategoryInfo cinfo;
    cinfo.registered = true;
    cinfo.cat = cat;
    cinfo.name = name;
    cinfo.simulation = simulation;
    cinfo.dim = dim;
    for (int i = 0; i < dim; ++i)
        cinfo.sizes[i] = sizes[i];
    cinfo.ptr = nullptr;

    cinfovec[pos] = cinfo;

//     printf("Register category %d with name=%s\n", cat, cinfo.name.c_str());
    return true;
}

/** Init all braches in the output tree.
 *
 * Uses registered categories info to init branches. If the category is
 * persistent, will be written to output tree. The persistent input category
 * will be rewritten to output tree.
 */
void SiFiManager::initBranches()
{
    size_t limit = SCategory::CatLimitDoNotUse * 2;
    for (size_t i = 0; i < limit; ++i)
    {
        CategoryInfo cinfo = cinfovec[i];
        if (!cinfo.persistent)
            continue;

        outputTree->Branch(cinfo.ptr->getName(), cinfo.ptr, 16000, 2);
    }

    branches_set = true;
}

/** Build category based on its ID. Category must be first registered.
 *
 * \param cat category ID
 * \param persistent set category persistent
 * \return pointer to category object
 */
SCategory * SiFiManager::buildCategory(SCategory::Cat cat, bool persistent)
{
    if (!outputTree)
        return gNullSCategoryPtr;

    int pos = cat * (1+(int)sim);

    CategoryInfo cinfo = cinfovec[pos];
    if (cinfo.registered == false)
        return gNullSCategoryPtr;

    cinfo.persistent = persistent;
    SCategory * cat_ptr = new SCategory(cinfo.name.c_str(), cinfo.dim, cinfo.sizes, cinfo.simulation);
    cinfo.ptr = cat_ptr;
    cat_ptr->print();

    if (cat_ptr)
    {
        categories[cat] = cat_ptr;
        cinfovec[pos] = cinfo;
        return cat_ptr;
    }

    return gNullSCategoryPtr;
}

/** Get the category.
 *
 * If category is not open, opens the category from input tree.
 *
 * \param cat category ID
 * \param persistent set category persistent
 * \return pointer to category object
 */
SCategory * SiFiManager::getCategory(SCategory::Cat cat, bool persistent)
{
    int pos = cat * (1+(int)sim);
    CategoryInfo cinfo = cinfovec[pos];
    if (cinfo.registered == false)
        return gNullSCategoryPtr;
    if (cinfo.ptr)
        return cinfo.ptr;
    SCategory * c = openCategory(cat, persistent);
    if (c)
        return c;

    return gNullSCategoryPtr;
}

/** Open the category from input tree.
 *
 * \param cat category ID
 * \param persistent set category persistent
 * \return pointer to category object
 */
SCategory * SiFiManager::openCategory(SCategory::Cat cat, bool persistent)
{
    if (!inputTree)
        return gNullSCategoryPtr;

    int pos = cat * (1+(int)sim);

    CategoryInfo cinfo = cinfovec[pos];
    if (cinfo.registered == false)
    {
        fprintf(stderr, "Category %s (%d) not registered!\n", cinfo.name.c_str(), cat);
        return gNullSCategoryPtr;
    }

    if (categories[cat])
            return categories[cat];

    SCategory ** cat_ptr = new SCategory*;
    *cat_ptr = new SCategory;

    TBranch ** br = new TBranch*;

    Int_t res = inputTree->SetBranchAddress(cinfo.name.c_str(), &*cat_ptr, br);
    if (!br)
    {
        fprintf(stderr, "Category %s (%d) does not exists!\n", cinfo.name.c_str(), cat);
        return gNullSCategoryPtr;
    }

    cinfo.persistent = persistent;
    if (*cat_ptr)
    {
        cinfo.ptr = *cat_ptr;
        cinfovec[pos] = cinfo;

        categories[cat] = *cat_ptr;
        return *cat_ptr;
    }
    else
        return gNullSCategoryPtr;
}

/** Reads entry of the current tree.
 *
 * The categories are filled with the data saved in tree entry i.
 *
 * \param i event number
 */
void SiFiManager::getEntry(int i)
{
    if (!inputTree)
    {
        std::cerr << "[Warning] in SiFiManager: no input tree is opened. cannot get any entry." << "\n";
        return;
    }

    if (i < numberOfEntries)
    {
        currentEntry = i;
        inputTree->GetEntry(i);
    }
}

/** Returns number of entries in the current tree.
 *
 * \return number of entries
 */
Long64_t SiFiManager::getEntries()
{
    if (!inputTree)
    {
        std::cerr << "[Warning] in SiFiManager: no input tree is opened. cannot get any entry." << "\n";
        return -1;
    }
    numberOfEntries = inputTree->GetEntries();
    return numberOfEntries;
}

/** Print info about the categories.
 */
void SiFiManager::print() const
{
    outputFile->cd();
    printf("There are %ld categories in the output tree\n", categories.size());
    for (CatMap::const_iterator it = categories.begin(); it != categories.end(); ++it)
    {
        it->second->print();
    }
}

/** Clear all categories.
 */
void SiFiManager::clear()
{
    for (CatMap::const_iterator it = categories.begin(); it != categories.end(); ++it)
    {
        it->second->clear();
    }
}

/** Constructor
 */
SiFiManager::CategoryInfo::CategoryInfo()
{
    registered = false;
    persistent = false;
    ptr = nullptr;
}

/** Copy constructor
 */
SiFiManager::CategoryInfo::CategoryInfo(SiFiManager::CategoryInfo& ci)
{
    registered = ci.registered;
    cat = ci.cat;
    name = ci.name;
    simulation = ci.simulation;
    dim = ci.dim;
    for (int i = 0; i < 16; ++i)
        sizes[i] = ci.sizes[i];
    persistent = ci.persistent;
    ptr = ci.ptr;
}

ClassImp(SiFiManager);
