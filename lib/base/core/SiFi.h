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

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

#include "sifi_export.h"

#include "SCategory.h"
#include "SDataSource.h"
#include "SEvent.h"
#include "SRootFileHeader.h"

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include <fstream>
#include <map>

class SiFi
{
protected:
    // members
    TFile* outputFile;                          ///< Pointer to output file
    TTree* outputTree;                          ///< Pointer to output tree
    std::string outputTreeTitle;                ///< Output tree title
    std::string outputTreeName;                 ///< Output tree name
    std::string outputFileName;                 ///< Output file name
    SRootFileHeader fileHeader;                 ///< Root File Header
    TTree* inputTree;                          ///< Pointer to input tree
    std::string inputTreeTitle;                 ///< Input tree title
    std::string inputTreeName;                  ///< Input tree name
    std::vector<SDataSource *> inputSources;    ///< Input file name
    int numberOfEntries;                        ///< Number of input entries
    int currentEntry;                           ///< Current input entry number
    SEvent * event;

    /// Ctaegory info
    struct CategoryInfo
    {
        bool registered = false;    ///< Category is registered
        bool persistent = false;    ///< Category is persistent
        SCategory::Cat cat;         ///< Category ID
        std::string name;           ///< Category name
        bool simulation;            ///< Simulation run
        size_t dim;                 ///< Dimension of ctageory
        size_t sizes[16];           ///< Sizes of dimension
        SCategory * ptr = nullptr;  ///< Pointer to category object
    };

    /// Category info array
    CategoryInfo cinfovec[SCategory::CatLimitDoNotUse * 2];

    typedef std::map<SCategory::Cat, SCategory *> CatMap;
    CatMap categories;              ///< Map of categories
    static SiFi * mm;               ///< Instance of the SiFi
    bool sim;                       ///< Simulation run
    bool branches_set;              ///< Has branches set

private:
    // constructor
    SiFi();

public:
    SiFi(SiFi const &) = delete;
    SiFi & operator=(SiFi const &) = delete;

    // instance method
    SIFI_EXPORT static SiFi * instance();

    // destructor
    virtual ~SiFi() {};

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

    SIFI_EXPORT bool registerCategory(SCategory::Cat cat, const std::string & name, size_t dim, size_t * sizes, bool simulation);

    SIFI_EXPORT SCategory * buildCategory(SCategory::Cat cat, bool persistent = true);
    SIFI_EXPORT SCategory * getCategory(SCategory::Cat cat, bool persistent = true);

    /// Set output file name
    /// \param file file name
    void setOutputFileName(const std::string & file) { outputFileName = file; }

    void addSource(SDataSource * data) { inputSources.push_back(data); }
    /// Get entry number
    /// \return entry number
    int getEntryNumber() const { return currentEntry; }

    /// Get input tree
    /// \return input tree
    TTree * getTree() const { return inputTree; }
    void setTree(TTree * t) { inputTree = t; }

    SEvent * getCurrentEvent() const { return event; }
    void setEvent(SEvent * e);

    static int getCategoryIndex(SCategory::Cat cat, int simulation) {
        return (cat * 2) + (int)simulation; }

private:
    SIFI_NO_EXPORT void initBranches();
};

extern SIFI_EXPORT SiFi * sifi();

#endif /* SIFI_H */
