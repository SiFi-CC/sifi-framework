// @(#)lib/base/util:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SLOOP_H
#define SLOOP_H

#include "sifi_export.h"

#include "SCategory.h"

#include <initializer_list>

class SEvent;
class SRootFileHeader;
class TChain;
class TFile;
class TTree;

/** Loops over the tree, e.g. from the root file.
 * \todo Add support for SRootSource
 */
class SLoop
{
public:
    // constructor
    SLoop();
    // destructor
    virtual ~SLoop();

    bool addFile(const std::string & file);
    bool addFiles(const std::vector<std::string> & files);

    void setInput(std::initializer_list<SCategory::Cat> categories);

    size_t getEntries() const;
    Int_t nextEvent();
    Int_t getEvent(ulong event);

private:
    TChain * chain;                 ///< chain storing all input files
    TFile * current_file;           ///< pointer to the current file
    TTree * current_tree;           ///< pointer to the current tree
    ulong current_event;            ///< number of current event
    SCategory * categories[SCategory::CatLimitDoNotUse*2];  ///< list of categories

    SEvent * event;                 ///< event data
    SRootFileHeader * file_header;  ///< file header

    Long64_t tree_cache_size;       ///< tree cache size
};

#endif /* SLOOP_H */
