// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SiFi_H
#define SiFi_H

#define PR(x)                                                                                      \
    std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

#include "sifi_export.h"

#include "SCategory.h"
#include "SRootFileHeader.h"

#include "RtypesCore.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <vector>

class SDataSource;
class SEvent;
class TFile;
class TTree;

class SIFI_EXPORT SiFi
{
protected:
    // members
    TFile* outputFile;                      ///< Pointer to output file
    std::string outputFileName;             ///< Output file name
    TTree* outputTree;                      ///< Pointer to output tree
    std::string outputTreeName;             ///< Output tree name
    SRootFileHeader fileHeader;             ///< Root File Header
    TTree* inputTree;                       ///< Pointer to input tree
    std::string inputTreeName;              ///< Input tree name
    std::vector<SDataSource*> inputSources; ///< Input file name
    int numberOfEntries;                    ///< Number of input entries
    int currentEntry;                       ///< Current input entry number
    SEvent* event;                          ///< Event info structure

    /// Ctaegory info
    static struct CategoryInfo
    {
        bool registered = false;             ///< Category is registered
        bool persistent = false;             ///< Category is persistent
        SCategory::Cat cat;                  ///< Category ID
        std::string name;                    ///< Category name
        bool simulation;                     ///< Simulation run
        size_t dim;                          ///< Dimension of ctageory
        size_t sizes[16];                    ///< Sizes of dimension
        SCategory* ptr = nullptr;            ///< Pointer to category object
    } cinfovec[SCategory::CatLimitDoNotUse]; ///< Category info array

    std::map<SCategory::Cat, SCategory*> categories; ///< Map of categories
    static SiFi* mm;                                 ///< Instance of the SiFi
    bool sim;                                        ///< Simulation run
    bool branches_set;                               ///< Has branches set

private:
    // constructor
    SiFi();

public:
    SiFi(SiFi const&) = delete;
    SiFi& operator=(SiFi const&) = delete;

    // instance method
    SIFI_EXPORT static SiFi* instance();

    // destructor
    virtual ~SiFi();

    // methods
    void print() const;
    void clear();

    SIFI_EXPORT bool book(bool with_tree = true);
    SIFI_EXPORT bool save();
    Int_t fill();
    bool open();

    SIFI_EXPORT void loop(long entries, bool show_progress_bar = true);

    void getEntry(int i);
    SIFI_EXPORT Long64_t getEntries();

    void setSimulation(bool simulation);
    /// Check if simulation run
    /// \return is simulation
    bool isSimulation() const { return sim; }

    SIFI_EXPORT static bool registerCategory(SCategory::Cat cat, const std::string& name,
                                             size_t dim, size_t* sizes, bool simulation);
    SIFI_EXPORT static bool registerCategory(SCategory::Cat cat, const std::string& name, size_t n,
                                             bool simulation);

    SIFI_EXPORT SCategory* buildCategory(SCategory::Cat cat, bool persistent = true);
    SIFI_EXPORT SCategory* getCategory(SCategory::Cat cat, bool persistent = true);

    /// Set output file name
    /// \param file file name
    void setOutputFileName(const std::string& file) { outputFileName = file; }

    /// Add source to the list of sources
    /// \param data source object
    void addSource(SDataSource* data) { inputSources.push_back(data); }

    /// Get entry number
    /// \return entry number
    int getEntryNumber() const { return currentEntry; }

    /// Get input tree
    /// \return input tree
    TTree* getTree() const { return inputTree; }
    /// Set input tree
    /// \param t TTree tree object
    void setTree(TTree* t) { inputTree = t; }

    /// Returns curent event.
    /// \return SEvent structure
    SEvent* getCurrentEvent() const { return event; }
    void setEvent(SEvent* e);

    /// Maps category kind and simulation flag into index.
    /// \param cat category kind
    /// \param simulation simulation flag
    /// \return linearised index of the category
    static int getCategoryIndex(SCategory::Cat cat, int /*simulation*/) { return cat; }

private:
    SIFI_NO_EXPORT void initBranches();
};

extern SIFI_EXPORT SiFi* sifi();

#endif /* SIFI_H */
