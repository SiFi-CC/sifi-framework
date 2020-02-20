// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SiFiMANAGER_H
#define SiFiMANAGER_H

using namespace std;

#include <string>
#include <fstream>
#include <map>

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

#include "SCategory.h"

class SiFiManager: public TObject
{
protected:
    // members
    TFile* outputFile;                          ///< Pointer to output file
    TTree* outputTree;                          ///< Pointer to output tree
    string outputTreeTitle;                     ///< Output tree title
    string outputTreeName;                      ///< Output tree name
    string outputFileName;                      ///< Output file name
    TChain* inputTree;                          ///< Pointer to input tree
    string inputTreeTitle;                      ///< Input tree title
    string inputTreeName;                       ///< Input tree name
    std::vector<std::string> inputFiles;        ///< Input file name
    int numberOfEntries;                        ///< Number of input entries
    int currentEntry;                           ///< Current input entry number

    /// Ctaegory info
    struct CategoryInfo
    {
        CategoryInfo();
        CategoryInfo(CategoryInfo & ci);

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
    static SiFiManager * mm;       ///< Instance of the SiFiManager
    bool sim;                       ///< Simulation run
    bool branches_set;              ///< Has branches set

private:
    // constructor
    SiFiManager();
    SiFiManager(SiFiManager const &) {}   ///< Copy constructor
    /// Assignment operator
    /// \return this object
    SiFiManager & operator=(SiFiManager const &) { return *this; }

public:
    // instance method
    static SiFiManager * instance();

    // destructor
    virtual ~SiFiManager() {};

    // methods
    void print() const;
    void clear();

    bool book(bool with_tree = true);
    bool save();
    Int_t fill();
    bool open();

    void getEntry(int i);
    Long64_t getEntries();

    void setSimulation(bool simulation);
    /// Check if simulation run
    /// \return is simulation
    bool isSimulation() const { return sim; }

    bool registerCategory(SCategory::Cat cat, std::string name, size_t dim, size_t * sizes, bool simulation);

    SCategory * buildCategory(SCategory::Cat cat, bool persistent = true);
    SCategory * getCategory(SCategory::Cat cat, bool persistent = true);
    SCategory * openCategory(SCategory::Cat cat, bool persistent = true);

    /// Set output file name
    /// \param file file name
    void setOutputFileName(const std::string & file) { outputFileName = file; }
    void setInputFileName(const std::string & file);
    void addInputFileName(const std::string & file);
    void setInputFileNames(const std::vector<std::string> & files);
    void addInputFileNames(const std::vector<std::string> & files);

    /// Get entry number
    /// \return entry number
    int getEntryNumber() { return currentEntry; }

    /// Get input tree
    /// \return input tree
    TChain * getTree() const { return inputTree; }

private:
    void initBranches();

    ClassDef(SiFiManager, 1);
};

extern SiFiManager * mapt();

#endif /* DATAMANAGER_H */
