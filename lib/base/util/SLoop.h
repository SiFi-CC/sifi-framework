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
    int nextEvent(uint ev);

    virtual void print();

private:
    TChain * chain;
    TFile * current_file;
    TTree * current_tree;
    uint current_event;
    SCategory ** categories;

    SEvent * event;
    SRootFileHeader * fileHeader;

    Long64_t tree_cache_size;
};

#endif /* SLOOP_H */
