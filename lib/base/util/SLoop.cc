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
#include "SiFi.h"
#include "SEvent.h"
#include "SRootFileHeader.h"
#include "SProgressBar.h"

#include <TChain.h>
#include <TFile.h>
#include <TSystem.h>

#include <cstdio>
#include <iostream>
#include <map>

/**
 * \class SLoop
\ingroup lib_base_util

*/

SLoop::SLoop()
    : current_file(nullptr)
    , current_tree(nullptr)
    , current_event(0)
    , event(nullptr)
    , file_header(nullptr)
    , tree_cache_size(8000)
{
    chain = new TChain("S");
    event = new SEvent;

    sifi()->setTree(chain);
    sifi()->setEvent(event);
}

SLoop::~SLoop()
{
    delete event;
    delete chain;
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
    } else {
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
    for (const auto &file : files)
    {
        bool rc = addFile(file);
        if (rc)
            return false;
    }

    return true;
}

/**
 * Set categories to be read from the input files.
 *
 * \param categories list of categories to be read.
 */
void SLoop::setInput(std::initializer_list<SCategory::Cat> categories)
{
    if (chain->GetListOfFiles()->GetEntries() == 0) abort();

    current_event = 0;
    chain->GetEntry(current_event);

    current_tree = chain->GetTree();
    current_tree->Print();

    TFile * f = chain->GetCurrentFile();
    f->GetObject("FileHeader", file_header);

    CatNameMap::iterator iter = file_header->catName.begin();
    for (; iter != file_header->catName.end(); ++iter)
    {
        printf("Read category %s\n", iter->second.Data());

        TBranch * br = current_tree->GetBranch(Form("%s", iter->second.Data())); // FIXME add .
        if(!br)
        {
    //         Error("setInput()","Branch not found : %s !",Form("%s.",catname.Data()));
    //         return kFALSE;
        }
        else
        {
            int pos = SiFi::getCategoryIndex(iter->first, sifi()->isSimulation());
            chain->SetBranchAddress(Form("%s", iter->second.Data()), &(this->categories[pos]));
            chain->SetBranchStatus (Form("%s", iter->second.Data()),1);
            chain->SetBranchStatus (Form("%s", iter->second.Data()),1);

            sifi()->getCurrentEvent()->addCategory(iter->first, this->categories[pos]);
        }
    }
}

/**
 * Returns numer of entries in the chain.
 *
 * \return number of entries
 */
size_t SLoop::getEntries() const
{
    return chain->GetEntries();
}

/**
 * Fetch the next event from the #chain. If the event rach end of teh tree, it
 * return 0. See TChain::GetEntry() for details.
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
 * it returns 0. See TChain::GetEntry() for details.
 *
 * \param event event number
 * \return 0 indicates error, >0 is a number of read bytes.
 */
Int_t SLoop::getEvent(ulong event) {
    sifi()->getCurrentEvent()->clearCategories();
    return chain->GetEntry(event);
}
