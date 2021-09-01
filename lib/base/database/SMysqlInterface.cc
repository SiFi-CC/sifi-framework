// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "SMysqlInterface.h"
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
#include <TObjArray.h>
#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility> // for pair, make_pair, move
#include <vector>

/**
 * \class SMysqlInterface

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

std::time_t extract_date(std::string date_str)
{
    std::tm t = {0};
    std::istringstream ss(date_str);
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M");
    return mktime(&t);
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

SMysqlInterface::SMysqlInterface(std::string_view url, std::string_view token)
    : api_url(url), auth_token(token)
{
    cpr::Response r = cpr::Get(cpr::Url{std::string(api_url) + "/api/connection"},
                               cpr::Header{{"Authorization", "Token " + std::string(auth_token)}});
    connection_ok = (r.status_code == 204);
}

std::optional<SRunContainer> SMysqlInterface::getRunContainer(long runid)
{
    cpr::Response r =
        cpr::Get(cpr::Url{std::string(api_url) + "/api/fetch/run/" + std::to_string(runid)},
                 cpr::Header{{"Authorization", "Token " + std::string(auth_token)}});

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
        int run_id = d["run_id"].GetInt();

        assert(runid == run_id);

        std::time_t valid_from = extract_date(
            std::string(d["start_time"].GetString(), d["start_time"].GetStringLength()));
        std::time_t valid_to =
            extract_date(std::string(d["stop_time"].GetString(), d["stop_time"].GetStringLength()));

        SRunContainer run_container;
        run_container.setId(run_id);
        run_container.setStart(valid_from);
        run_container.setStop(valid_to);
        run_container.setType(d["run_type"].GetInt());
        return {std::move(run_container)};
    }
    return std::nullopt;
}

/**
 * Return run containers from given run id range. If both arguments are 0,
 * then take all runs from given release.
 * \param rinid_min minimal runid, weak ordering
 * \param runid_max maximal runid, strong ordering
 * \return array of objects, empty if no matches
 */
TObjArray SMysqlInterface::getRunContainers(long runid_min, long runid_max)
{
    cpr::Response r;

    if (runid_min == 0 and runid_max == 0)
        r = cpr::Get(cpr::Url{std::string(api_url) + "/api/fetch/run/" + param_release},
                     cpr::Header{{"Authorization", "Token " + std::string(auth_token)}});
    else
        r = cpr::Get(cpr::Url{std::string(api_url) + "/api/fetch/run/" + std::to_string(runid_min) +
                              "/" + std::to_string(runid_max)},
                     cpr::Header{{"Authorization", "Token " + std::string(auth_token)}});

    TObjArray array;
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
            std::time_t valid_from = extract_date(
                std::string(d["start_time"].GetString(), d["start_time"].GetStringLength()));
            std::time_t valid_to = extract_date(
                std::string(d["stop_time"].GetString(), d["stop_time"].GetStringLength()));

            SRunContainer* run_container = new SRunContainer;
            run_container->setId(d["run_id"].GetInt());
            run_container->setStart(valid_from);
            run_container->setStop(valid_to);
            run_container->setType(d["run_type"].GetInt());
            array.Add(run_container);
        }
    }
    return array;
}

/**
 * Add new run contaniner. The runid is stored inside the container.
 * \param runcont run container to add.
 */
void SMysqlInterface::addRunContainer(SRunContainer&& runcont)
{
    std::time_t valid_from = runcont.getStart();
    std::time_t valid_to = runcont.getStop();
    auto run_id = runcont.getId();

    char valid_from_str[256];
    char valid_to_str[256];

    std::strftime(valid_from_str, sizeof(valid_from_str), "%Y-%m-%dT%H:%M", gmtime(&valid_from));
    std::strftime(valid_to_str, sizeof(valid_to_str), "%Y-%m-%dT%H:%M", gmtime(&valid_to));

    rapidjson::StringBuffer s;
    rapidjson::Writer<rapidjson::StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("start_time");
    writer.String(valid_from_str);
    writer.Key("stop_time");
    writer.String(valid_to_str);
    writer.Key("run_id");
    writer.Int(run_id);
    writer.EndObject();

    std::string strJson = s.GetString();

    cpr::Response r = cpr::Post(cpr::Url{std::string(api_url) + "runs/"}, cpr::Body{strJson},
                                cpr::Header{{"Authorization", "Token " + std::string(auth_token)},
                                            {"Content-Type", "application/json"}});
}

std::optional<SContainer> SMysqlInterface::getContainer(std::string_view&& name, long runid)
{
    cpr::Response r =
        cpr::Get(cpr::Url{std::string(api_url) + "/api/fetch/cont/" + param_release + "/" +
                          std::string(name) + "/" + std::to_string(runid) + "/"},
                 cpr::Header{{"Authorization", "Token " + std::string(auth_token)}});

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
        int run_id = d["run_id"].GetInt();
        assert(runid == run_id);

        uint32_t valid_from = d["valid_from_id"].GetInt();
        uint32_t valid_to = d["valid_to_id"].GetInt();

        SContainer cont;
        cont.lines = split_lines(d["content"].GetString());
        cont.updated = true;
        cont.validity = {valid_from, valid_to};
        return {std::move(cont)};
    }
    return std::nullopt;
}

/**
 * Return named containers from given run id range, from min until now/end.
 * \param name container name
 * \param rinid_min minimal runid, weak ordering
 * \return array of objects, empty if no matches
 */
std::vector<SContainer> SMysqlInterface::getContainers(std::string_view&& name, long runid_min)
{
    return getContainers(std::move(name), runid_min, 0);
}

/**
 * Return named containers from given run id range.
 * \param name container name
 * \param rinid_min minimal runid, weak ordering
 * \param runid_max maximal runid, strong ordering
 * \return array of objects, empty if no matches
 */
auto SMysqlInterface::getContainers(std::string_view&& name, long runid_min, long runid_max)
    -> std::vector<SContainer>
{
    cpr::Response r = cpr::Get(cpr::Url{std::string(api_url) + "/api/fetch/cont/" + param_release +
                                        "/" + std::string(name) + "/" + std::to_string(runid_min) +
                                        "/" + std::to_string(runid_max) + "/"},
                               cpr::Header{{"Authorization", "Token " + std::string(auth_token)}});

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
            int run_id = d["run_id"].GetInt();
            assert(runid == run_id);

            uint32_t valid_from = d["valid_from_id"].GetInt();
            uint32_t valid_to = d["valid_to_id"].GetInt();

            SContainer cont;
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
bool SMysqlInterface::addContainer(std::string_view&& name, SContainer&& cont) { return false; }
