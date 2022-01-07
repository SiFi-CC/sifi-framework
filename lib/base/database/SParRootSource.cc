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

#include "SContainer.h"
#include "SDatabase.h"

#include <TClass.h>      // for TClass
#include <TCollection.h> // for TIter
#include <TDirectory.h>  // for TDirectory, gDirectory
#include <TFile.h>
#include <TKey.h>
#include <TList.h>
#include <TObject.h> // for TObject

#include "tabulate/cell.hpp"           // for Cell
#include "tabulate/color.hpp"          // for Color, Color::red, Color::yellow
#include "tabulate/column.hpp"         // for Column, ColumnFormat::font_align
#include "tabulate/column_format.hpp"  // for ColumnFormat
#include "tabulate/font_align.hpp"     // for FontAlign, FontAlign::center
#include "tabulate/font_style.hpp"     // for FontStyle, FontStyle::bold
#include "tabulate/format.hpp"         // for Format
#include "tabulate/row.hpp"            // for Row
#include "tabulate/table_internal.hpp" // for Cell::format

#include <cstring> // for strcmp
#include <ctime>
#include <cxxabi.h> // for __forced_unwind
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <utility> // for pair
#include <variant> // for variant

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
SParRootSource::SParRootSource(const std::string& source) : SParSource(), source(source) {}

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
SParRootSource::SParRootSource(std::string&& source) : SParSource(), source(source) {}

auto SParRootSource::doSetOpenMode(SourceOpenMode mode) -> void
{
    switch (mode)
    {
        case SourceOpenMode::Input:
            file_source = TFile::Open(source.c_str(), "READ");
            if (!file_source) file_source = TFile::Open(source.c_str(), "RECREATE");
            parseSource();
            break;
        case SourceOpenMode::Output:
            file_source = TFile::Open(source.c_str(), "RECREATE");
            break;
    }
}

/**
 * Parse source file. Implemented based on hadd.C from ROOT.
 *
 * \return success
 */
bool SParRootSource::parseSource()
{
    file_source->cd();
    TDirectory* current_sourcedir = gDirectory;
    // gain time, do not add the objects in the list in memory
    // Bool_t status = TH1::AddDirectoryStatus();
    // TH1::AddDirectory(kFALSE);

    // loop over all keys in this directory
    TIter nextkey(current_sourcedir->GetListOfKeys());
    TKey *key, *oldkey = 0;
    while ((key = (TKey*)nextkey()))
    {
        // read object from first source file
        file_source->cd();
        TObject* obj = key->ReadObj();

        printf("Obj name = %s\n", obj->GetName());

        if (obj->IsA()->InheritsFrom(TCollection::Class()))
        {
            std::string name = obj->GetName();
            auto& cont_map = containers[name];
            TIter next(dynamic_cast<TCollection*>(obj));
            SContainer* cache = nullptr;

            while (TObject* c = next())
            {
                printf(" C name = %s\n", c->GetName());
                SContainer* cont = dynamic_cast<SContainer*>(c);
                if (cont)
                {
                    //                     if (cache) cache->validity.truncate(cont->validity);
                    cache = cont;
                }
            }

            next = TIter(dynamic_cast<TCollection*>(obj));

            while (TObject* c = next())
            {
                auto cont = std::shared_ptr<SContainer>(dynamic_cast<SContainer*>(c));
                if (cont) { cont_map.insert({cont->validity, cont}); }
            }
        }
    }
    return false;
}

auto SParRootSource::doFindContainer(const std::string& name) -> bool
{
    // check if same release
    auto exp = SRuntimeDb::get()->getExperiment();
    // TODO do we need to verify Experiment for root files or we just assume that they are always
    // correct (user responsibility)

    // TODO if release has name, then check whether it matches the one from file
    // if (!release.empty() and release != this_release_from_file) return 0;

    // check if container is in the source at all
    auto it = containers.find(name);
    if (it != containers.end()) return true;

    return false;
}

auto SParRootSource::doGetContainer(const std::string& name, ulong runid)
    -> std::shared_ptr<SContainer>
{
    // check if same release
    auto exp = SRuntimeDb::get()->getExperiment();
    // TODO if release has name, then check whether it matches the one from file
    // if (!release.empty() and release != this_release_from_file) return 0;

    // check if container is in the source at all
    auto it = containers.find(name);
    if (it == containers.end()) { return nullptr; }

    // if it was the same version like before, return cached one
    auto last = last_container[name];
    if (last and last->validity.inside(runid)) return last;

    // get fresh version, need to set flag reinit! TODO

    auto&& cont_map = containers[name];
    auto it2 = cont_map.lower_bound(SRunsValidityRange(runid, runid));
    if (it2 != cont_map.end())
    {
        if (it2->second->validity.inside(runid))
        {
            // TODO force DB to reinit here
            return it2->second;
        }
    }

    return nullptr;
}

auto SParRootSource::doInsertContainer(const std::string& name, SContainer* cont) -> bool {}

auto SParRootSource::doInsertContainer(const std::string& name, std::vector<SContainer*> cont)
    -> bool
{
    if (file_source)
    {
        file_source->cd();
        auto list = new TObjArray;
        list->SetName(name.c_str());

        for (auto& c : cont)
            list->Add(c);

        list->Write(name.c_str(), TObject::kSingleKey);
        return true;
    }

    return false;
}

auto SParRootSource::doGetRuns() -> std::vector<SRun>
{
    // check if same release
    auto exp = SRuntimeDb::get()->getExperiment();
    if (experiment.value_or(SExperiment()).name != exp) return {};

    std::vector<SRun> ret;
    for (auto& r : runs)
        ret.push_back(r.second);

    return ret;
}

auto SParRootSource::doGetRun(ulong runid) -> SRun
{
    auto it = runs.find(runid);
    if (it != runs.end()) return it->second;

    return {};
}

auto SParRootSource::doInsertRun(SRun run) -> bool
{
    if (runs.find(run.getId()) == runs.end())
    {
        runs[run.getId()] = run;

        if (file_source)
        {
            file_source->cd();
            run.Write();
            return true;
        }
    }
    return false;
}

auto SParRootSource::doGetExperiment() const -> std::optional<SExperiment> { return {}; }

auto SParRootSource::getDirectory(const std::string& name) -> TDirectory*
{
    return file_source->GetDirectory(name.c_str());
}

auto SParRootSource::createDirectory(const std::string& name) -> TDirectory*
{
    return file_source->mkdir(name.c_str());
}

#include <tabulate/table.hpp>
using namespace tabulate;

#include <magic_enum.hpp>

void SParRootSource::doPrint() const
{
    std::cout << "=== ROOT Source Info ===" << std::endl;
    std::cout << "    File name: " << source << std::endl;

    for (auto& container : containers)
    {
        const SRunsValidityRange* cache = nullptr;
        Table cont_summary;
        cont_summary.add_row({container.first, "Valid from", "Valid to", "Overlap", "Truncated"});

        for (auto& revision : container.second)
        {
            //             std::stringstream s_from;
            //             s_from << std::put_time(std::gmtime(&revision.first.from), "%c %Z");
            //
            //             std::stringstream s_to;
            //             s_to << std::put_time(std::gmtime(&revision.first.to), "%c %Z");

            std::stringstream trunc_from;
            if (revision.first.truncated > 0)
                trunc_from << std::put_time(std::gmtime(&revision.first.truncated), "%c %Z");

            bool overlap = cache ? revision.first.check_overlap(*cache) : false;

            //             cont_summary.add_row(
            //                 {"", s_from.str(), s_to.str(), std::to_string(overlap),
            //                 trunc_from.str()});

            cont_summary.add_row({"", std::to_string(revision.first.from),
                                  std::to_string(revision.first.to), std::to_string(overlap),
                                  trunc_from.str()});

            cache = &revision.first;
        }

        cont_summary.column(3).format().font_align(FontAlign::center);
        cont_summary.column(4).format().font_align(FontAlign::center).font_color(Color::red);

        for (size_t i = 0; i < 5; ++i)
        {
            cont_summary[0][i]
                .format()
                .font_color(Color::yellow)
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold});
        }

        std::cout << cont_summary << std::endl;
    }

    Table runs_summary;
    runs_summary.add_row({"Run", "Validated", "Start", "Stop"});

    std::cout << "Number of run containers: " << runs.size() << "\n";

    for (auto& r : runs)
    {
        auto t1 = r.second.getStart();
        auto t2 = r.second.getStop();
        std::stringstream s1;
        s1 << std::put_time(std::gmtime(&t1), "%c %Z");
        std::stringstream s2;
        s2 << std::put_time(std::gmtime(&t2), "%c %Z");
        runs_summary.add_row({std::to_string(r.second.getId()),
                              std::string(magic_enum::enum_name(r.second.getStatus())), s1.str(),
                              s2.str()});
    }

    std::cout << runs_summary << std::endl;
}
