// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SRESTInterface.h"
#include "SCalContainer.h"
#include "SContainer.h"
#include "SLookup.h"
#include "SPar.h"
#include "SParContainer.h"
#include "SParSource.h"
#include "SiFi.h"

#include "cpr/cpr.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility> // for pair, make_pair, move
#include <vector>

/**
 * \class SRESTInterface

\ingroup lib_base

*/

// using namespace rapidjson;

struct Parameter
{
    std::string param;
    std::string valid_from;
    std::string valid_to;
    std::size_t version;
};

/**
 * Return run container for given runid.
 * \param runid run id
 * \return object or empty slot
 */

std::time_t date_from_string(std::string date_str)
{
    std::tm t = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");
    return mktime(&t) - timezone;
}

std::string date_to_string(std::time_t date)
{
    char date_str[80];
    std::strftime(date_str, sizeof(date_str), "%FT%TZ", gmtime(&date));
    return std::string(date_str);
}

std::vector<std::string> split_lines(const std::string& s)
{
    std::vector<std::string> v;
    std::stringstream iss(s);

    while (iss.good())
    {
        std::string line;
        getline(iss, line, '\n');
        v.push_back(line);
    }
    return v;
}

// from cppreference about std::visitor
// https://en.cppreference.com/w/cpp/utility/variant/visit
// helper type for the visitor
template <class... Ts> struct db_type_overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> db_type_overloaded(Ts...) -> db_type_overloaded<Ts...>;

cpr::Session initSession(SIFI::Auth::auth_variant_t auth_data)
{
    cpr::Session session = cpr::Session();

    std::visit(
        db_type_overloaded{
            [&](const SIFI::Auth::BasicAuth& arg) {
                session.SetAuth({arg.user, arg.password, cpr::AuthMode::BASIC});
            },
            [&](const SIFI::Auth::TokenAuth& arg) {
                session.SetHeader(cpr::Header{{"Authorization", "Token " + arg.token}});
            },
            [](const SIFI::Auth::OnDemandAuth& /*arg*/) { printf("ONDEMAND AUTH\n"); }},
        auth_data);

    return session;
}

template <typename... Args>
cpr::Response buildGetSession(SIFI::Auth::auth_variant_t auth_data, cpr::Url url,
                              Args const&... args)
{
    auto&& session = initSession(auth_data);
    session.SetUrl(url);
    ((session.SetOption(args)), ...);
    return session.Get();
}

template <typename... Args>
cpr::Response buildPostSession(SIFI::Auth::auth_variant_t auth_data, cpr::Url url,
                               Args const&... args)
{
    auto&& session = initSession(auth_data);
    session.SetUrl(url);
    ((session.SetOption(args)), ...);
    return session.Post();
}

SRESTInterface::SRESTInterface(std::string url) : api_url(url) {}

// SRESTInterface::SRESTInterface(std::string url, std::string token) : api_url(url),
// auth_token(token)
// {
//     cpr::Response r = cpr::Get(cpr::Url{api_url + "/connection"},
//                                cpr::Header{{"Authorization", "Token " + auth_token}});
//     connection_ok = (r.status_code == 204);
// }
//
// SRESTInterface::SRESTInterface(std::string url, std::string user, std::string password) :
// api_url(url), auth_token(user)
// {
//     cpr::Response r = cpr::Get(cpr::Url{api_url + "/connection"},
//                                cpr::Header{{"Authorization", "Token " + auth_token}});
//     connection_ok = (r.status_code == 204);
// }

auto SRESTInterface::connected() const -> bool
{
    cpr::Response r = buildGetSession(auth_data, {api_url + "/connection"});
    return (r.status_code == 204);
};

// auto SRESTInterface::getRequest(cpr::Session && session) -> cpr::Response {
//
// }
//
// auto SRESTInterface::postRequest(cpr::Session && session) -> cpr::Response {
// }

auto SRESTInterface::getExperimentContainer(std::string exp) -> std::optional<SExperiment>
{
    cpr::Response r = buildGetSession(auth_data, {api_url + "/exp/" + exp + "/fetch"});

    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        auto release = SExperiment();
        release.name = doc["name"].GetString();
        release.start_date = doc["start_date"].GetString();
        release.close_date = doc["close_date"].IsNull() ? "" : doc["close_date"].GetString();

        return release;
    }
    return std::nullopt;
}

auto SRESTInterface::getRunContainer(std::string exp, long runid) -> SRun
{
    cpr::Response r = buildGetSession(
        auth_data, {api_url + "/exp/" + exp + "/run/" + std::to_string(runid) + "/fetch"});

    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        // data are stored as an array
        assert(doc.IsArray() and doc.Size() == 1);

        auto& d = doc[0];
        int run_id = d["id"].GetInt();

        assert(runid == run_id);

        SRun run;
        run.id = run_id;
        run.start_time = date_from_string(
            std::string(d["start_time"].GetString(), d["start_time"].GetStringLength()));
        ;
        run.stop_time = date_from_string(
            std::string(d["stop_time"].GetString(), d["stop_time"].GetStringLength()));
        ;
        run.type = d["run_type"].GetInt();
        return run;
    }
    return SRun{};
    ;
}

/**
 * Return run containers from given run id range. If both arguments are 0,
 * then take all runs from given release.
 * \return array of objects, empty if no matches
 */
auto SRESTInterface::getRunContainers(std::string exp) -> std::vector<SRun>
{
    cpr::Response r = buildGetSession(auth_data, {api_url + "/exp/" + exp + "/run/fetch"});

    std::vector<SRun> array;
    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        // data are stored as an array
        assert(doc.IsArray());

        for (auto& d : doc.GetArray())
        {
            std::time_t valid_from = date_from_string(
                std::string(d["start_time"].GetString(), d["start_time"].GetStringLength()));
            std::time_t valid_to = date_from_string(
                std::string(d["stop_time"].GetString(), d["stop_time"].GetStringLength()));

            SRun run;
            run.id = d["id"].GetInt();
            run.start_time = valid_from;
            run.stop_time = valid_to;
            run.type = d["run_type"].GetInt();
            array.push_back(run);
        }
    }
    return array;
}

/**
 * Add new run contaniner. The runid is stored inside the container.
 * \param runcont run container to add.
 */
auto SRESTInterface::openRunContainer(int run_type, std::time_t start_time, std::string file_name)
    -> std::optional<SRun>
{
    cpr::Response&& r = buildPostSession(auth_data, {api_url + "/run/open"},
                                         cpr::Multipart{{"run_type", run_type},
                                                        {"start_time", date_to_string(start_time)},
                                                        {"file_name", file_name}});

    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        auto& d = doc[0];
        auto& fields = d["fields"];
        SRun run;
        run.id = d["pk"].GetInt();
        run.start_time = date_from_string(
            std::string(fields["start_time"].GetString(), fields["start_time"].GetStringLength()));
        run.stop_time = fields["stop_time"].IsNull()
                            ? 0
                            : date_from_string(std::string(fields["stop_time"].GetString(),
                                                           fields["stop_time"].GetStringLength()));
        run.type = fields["run_type"].GetInt();
        run.status = fields["validated"].IsNull()    ? SRun::Status::Invalid
                     : fields["validated"].GetBool() ? SRun::Status::Valid
                                                     : SRun::Status::Invalid;
        return run;
    }
    else { printf("ERROR = %s\n", r.text.c_str()); }

    return std::nullopt;
}

/**
 * Add new run contaniner. The runid is stored inside the container.
 * \param runcont run container to add.
 */
auto SRESTInterface::closeRunContainer(std::time_t stop_time) -> std::optional<SRun>
{
    cpr::Response&& r = buildPostSession(auth_data, {api_url + "/run/close"},
                                         cpr::Multipart{{"stop_time", date_to_string(stop_time)}});

    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        auto& d = doc[0];
        auto& fields = d["fields"];
        SRun run;
        run.id = d["pk"].GetInt();
        run.start_time = date_from_string(
            std::string(fields["start_time"].GetString(), fields["start_time"].GetStringLength()));
        run.stop_time = fields["stop_time"].IsNull()
                            ? 0
                            : date_from_string(std::string(fields["stop_time"].GetString(),
                                                           fields["stop_time"].GetStringLength()));
        run.type = fields["run_type"].GetInt();
        run.status = fields["validated"].IsNull()    ? SRun::Status::Invalid
                     : fields["validated"].GetBool() ? SRun::Status::Valid
                                                     : SRun::Status::Invalid;

        return run;
    }
    else { printf("ERROR = %s\n", r.text.c_str()); }

    return std::nullopt;
}

auto SRESTInterface::findContainer(std::string exp, std::string name) -> bool
{
    cpr::Response r =
        buildGetSession(auth_data, {api_url + "/exp/" + exp + "/cont/" + name + "/find"});

    return r.status_code == 200;
}

std::optional<SContainer> SRESTInterface::getContainer(std::string exp, std::string name,
                                                       long runid)
{
    cpr::Response r = buildGetSession(auth_data, {api_url + "/exp/" + exp + "/cont/" + name +
                                                  "/run/" + std::to_string(runid) + "/fetch"});

    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        // data are stored as an array
        assert(doc.IsArray() and doc.Size() == 1);

        auto& d = doc[0];
        // int run_id = d["id"].GetInt();

        uint32_t valid_from = d["valid_from_id"].GetInt();
        uint32_t valid_to = d["valid_to_id"].GetInt();

        SContainer cont;
        cont.SetName(name.c_str());
        cont.lines = split_lines(d["content"].GetString());
        cont.updated = true;
        cont.validity = {valid_from, valid_to};
        cont.version = d["version"].GetInt();
        return cont;
    }
    return std::nullopt;
}

/**
 * Return named containers from given run id range.
 * \param name container name
 * \return array of objects, empty if no matches
 */
auto SRESTInterface::getContainers(std::string exp, std::string name) -> std::vector<SContainer>
{
    cpr::Response r =
        buildGetSession(auth_data, {api_url + "/exp/" + exp + "/cont/" + name + "/fetch"});

    std::vector<SContainer> array;
    if (r.status_code == 200)
    {
        // full json response
        const char* json = r.text.c_str();
        rapidjson::Document top_doc;
        top_doc.Parse(json);
        rapidjson::Document doc;
        doc.Parse(top_doc.GetString());

        // data are stored as an array
        assert(doc.IsArray() and doc.Size() == 1);

        for (auto& d : doc.GetArray())
        {
            uint32_t valid_from = d["valid_from_id"].GetInt();
            uint32_t valid_to = d["valid_to_id"].GetInt();

            SContainer cont;
            cont.SetName(name.c_str());
            cont.lines = split_lines(d["content"].GetString());
            cont.updated = true;
            cont.validity = {valid_from, valid_to};

            array.push_back(cont);
        }
    }
    return array;
}

/**
 * Add new container to the database, by name. It must be later validated via web interface.
 * \return success of the operation
 */
bool SRESTInterface::addContainer(std::string exp, std::string /*name*/, SContainer&& /*cont*/)
{
    return false;
}
