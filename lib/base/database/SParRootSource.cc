// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SParRootSource.h"

#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TList.h>

#include <algorithm>
#include <functional>

/**
 * \class SParRootSource

\ingroup lib_base_database

Reads parameters from ascii file. Can be also used as a target to write parameters.
*/

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
SParRootSource::SParRootSource(const std::string& source) : SParSource(), source(source)
{
    parseSource();
}

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
SParRootSource::SParRootSource(std::string&& source) : SParSource(), source(source)
{
    parseSource();
}

/**
 * Parse source file. Implemented based on hadd.C from ROOT.
 *
 * \return success
 */
bool SParRootSource::parseSource()
{
    TFile* first_source = TFile::Open(source.c_str(), "READ");
    if (!first_source) return false;

    first_source->cd();
    TDirectory* current_sourcedir = gDirectory;
    // gain time, do not add the objects in the list in memory
    // Bool_t status = TH1::AddDirectoryStatus();
    // TH1::AddDirectory(kFALSE);

    // loop over all keys in this directory
    TIter nextkey(current_sourcedir->GetListOfKeys());
    TKey *key, *oldkey = 0;
    while ((key = (TKey*)nextkey()))
    {
        // keep only the highest cycle number for each key
        if (oldkey && !strcmp(oldkey->GetName(), key->GetName())) continue;

        // read object from first source file
        first_source->cd();
        TObject* obj = key->ReadObj();

        if (obj->IsA()->InheritsFrom(TList::Class()))
        {
            // descendant of TH1 -> merge it

            std::string name = obj->GetName();
            auto&& cont_map = containers[name];
            TIter next(dynamic_cast<TList*>(obj));
            while (TObject* c = next())
            {
                SContainer* cont = dynamic_cast<SContainer*>(c);
                if (cont) { cont_map.insert({cont->validity, cont}); }
            }
        }
    }
    return false;
}

SContainer* SParRootSource::getContainer(const std::string& name, long runid)
{
    // check if container is in the source at all
    auto it = containers.find(name);
    if (it == containers.end()) { return nullptr; }

    // if it was the same version like before, return cached one
    SContainer* last = last_container[name];
    if (last and last->validity == runid) return last;

    // get fresh version, need to set flag reinit! TODO
    // also runid -> time conversion
    auto time = runid;

    auto&& cont_map = containers[name];
    auto it2 = cont_map.lower_bound(validity_range_t(time, time));
    if (it2 != cont_map.end())
    {
        if (it2->second->validity == time)
        {
            // TODO force DB to reinit here
            return it2->second;
        }
    }

    return nullptr;
}

void SParRootSource::print() const
{
    std::cout << "=== Ascii Source Info ===" << std::endl;
    std::cout << "    File name: " << source << std::endl;
    SParSource::print();
}
