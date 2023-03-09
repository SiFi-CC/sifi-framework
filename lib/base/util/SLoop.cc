// @(#)lib/base/util:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SLoop.h"
#include "SEvent.h"
#include "SRootFileHeader.h"
#include "SiFi.h"

#include <TChain.h>
#include <TFile.h>
#include <TObjArray.h> // for TObjArray
#include <TString.h>   // for Form, TString
#include <TSystem.h>
#include <TTree.h> // for TTree

#include <cstdio>
#include <cstdlib> // for abort
#include <cstring> // for memset
#include <iostream>
#include <map>
#include <utility> // for pair

class TBranch;

/**
 * \class SLoop
\ingroup lib_base_util

*/

SLoop::SLoop()
    : chain(nullptr), current_file(nullptr), current_tree(nullptr), current_event(0), event(nullptr),
      file_header(nullptr), tree_cache_size(8000)
{
    chain = new TChain("S");
    event = new SEvent;
    
    sifi()->setTree(chain);
    sifi()->setEvent(event);

    memset(categories, 0, SCategory::CatLimitDoNotUse * 2 * sizeof(SCategory*));
}

SLoop::~SLoop()
{
    delete chain;
    delete event;
    sifi()->setTree(nullptr);
    sifi()->setEvent(nullptr);
}

/**
 * Add file to the loop.
 *
 * \param file file name
 * \return false - file not found, true - success
 */
bool SLoop::addFile(const std::string& file)
{
    if (gSystem->AccessPathName(file.c_str()) == 0)
    {
        std::cout << "Add file : " << file << std::endl;
        chain->AddFile(file.c_str());
        return true;
    }
    else
    {
        //         Error("addFile()","File = %s not found! Will be skipped .... ",infile.Data());
        return false;
    }
}

/**
 * Add files to the loop. Runs addFile() internally. First file which will fail
 * breaks the execution of the function.
 *
 * \param files vector of files name
 * \return false - file not found, true - success.
 */
bool SLoop::addFiles(const std::vector<std::string>& files)
{
    for (const auto& file : files)
    {
        bool rc = addFile(file);
        if (!rc) return false;
    }

    return true;
}

/**
 * Set categories to be read from the input files.
 *
 * \todo categories selection doesn't work yet
 * \param categories list of categories to be read.
 */
void SLoop::setInput(std::initializer_list<SCategory::Cat> categories)
{
    if (chain->GetListOfFiles()->GetEntries() == 0) abort();

    current_event = 0;
    chain->GetEntry(current_event);

    current_tree = chain->GetTree();

    TFile* f = chain->GetCurrentFile();
    f->GetObject("FileHeader", file_header);

    if (!file_header)
    {
        std::cerr << "File header does not exists!" << std::endl;
        abort();
    }

    for (auto& cn : file_header->catName)
    {
        printf("Read category %s\n", cn.second.Data());

        TBranch* br = current_tree->GetBranch(Form("%s", cn.second.Data())); // FIXME add .
        if (!br)
        {
            //         Error("setInput()","Branch not found : %s !",Form("%s.",catname.Data()));
            //         return kFALSE;
        }
        else
        {
            int pos = SiFi::getCategoryIndex(cn.first);
            chain->SetBranchAddress(Form("%s", cn.second.Data()), &(this->categories[pos]));
            chain->SetBranchStatus(Form("%s", cn.second.Data()), 1);
            chain->SetBranchStatus(Form("%s", cn.second.Data()), 1);

            sifi()->getCurrentEvent()->addCategory(cn.first, this->categories[pos]);
        }
    }
}

/**
 * Returns numer of entries in the chain.
 *
 * \return number of entries
 */
size_t SLoop::getEntries() const { return chain->GetEntries(); }

/**
 * Fetch the next event from the #chain. If the event reaches end of the tree,
 * it returns 0. This function should be used at the end of the loop, as it
 * moves to event #1. setInput() resets event position to 0.
 *
 * See TChain::GetEntry() for details.
 *
 * \return 0 indicates error, >0 is a number of read bytes.
 */
Int_t SLoop::nextEvent()
{
    sifi()->getCurrentEvent()->clearCategories();
    return chain->GetEntry(++current_event);
}

/**
 * Fetch the given event from the #chain. If event number is outside teh chain,
 * it returns 0. See TChain::GetEntry() for details. Sets SLoop::current_event to event.
 *
 * \param event event number
 * \return 0 indicates error, >0 is a number of read bytes.
 */
Int_t SLoop::getEvent(ulong event)
{
    current_event = event;
    sifi()->getCurrentEvent()->clearCategories();
    return chain->GetEntry(event);
}
