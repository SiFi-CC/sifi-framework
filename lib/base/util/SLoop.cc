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

/** \class SLoop
\ingroup lib_base_util

*/

SLoop::SLoop::SLoop()
    : current_file(nullptr)
    , current_tree(nullptr)
    , current_event(0)
    , event(nullptr)
    , tree_cache_size(8000)
    , fileHeader(nullptr)
{
    chain = new TChain("S");

    event = new SEvent;
    categories = new SCategory*[SCategory::CatLimitDoNotUse*2];
    sifi()->setTree(chain);
    sifi()->setEvent(event);
}

SLoop::~SLoop()
{
    delete [] categories;
    delete event;
    delete chain;
}

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

void SLoop::setInput(std::initializer_list<SCategory::Cat> categories)
{
    if (chain->GetListOfFiles()->GetEntries() == 0) abort();

    current_event = 0;
    chain->GetEntry(current_event);

    current_tree = chain->GetTree();
    current_tree->Print();

    TFile * f = chain->GetCurrentFile();
    f->GetObject("FileHeader", fileHeader);

    CatNameMap::iterator iter = fileHeader->catName.begin();
    for (; iter != fileHeader->catName.end(); ++iter)
    {
        printf("Read category %s\n", iter->second.Data());
// PR(Form("%s.", iter->second.Data()));
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

size_t SLoop::getEntries() const
{
    return chain->GetEntries();
}

int SLoop::nextEvent(uint ev)
{
    sifi()->getCurrentEvent()->clearCategories();
    current_event = ev;
    return chain->GetEntry(ev);
}


void SLoop::print()
{
}
