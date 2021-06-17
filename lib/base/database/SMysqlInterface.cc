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
#include "SLookup.h"
#include "SPar.h"
#include "SParContainer.h"
#include "SParSource.h"
#include "SContainer.h"
#include "SiFi.h"

#include <cstdlib> // for exit, EXIT_FAILURE, abort
#include <iostream>
#include <utility> // for pair, make_pair, move
#include <string>
#include <string_view>
#include <sstream>
#include <ctime>
#include <iomanip> 
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "cpr/cpr.h"
#include <TObjArray.h>



/**
 * \class SMysqlInterface

\ingroup lib_base

*/

using namespace rapidjson;

struct Parameter{
    std::string param;
    std::string valid_from;
    std::string valid_to;
    std::size_t version;
};


SMysqlInterface::SMysqlInterface(std::string_view api_url): api_url(api_url){

}
/**
 * Return run container for given runid.
 * \param runid run id
 * \return object or empty slot
 */

std::time_t extract_date(std::string date_str){
    std::tm t = {0};
    std::istringstream ss(date_str);
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M");
    return mktime(&t);
}

std::vector<std::string> split_lines(const std::string& s){
    std::vector<std::string> v;
    std::stringstream iss(s);
    
    while(iss.good())
    {
        std::string line;
        getline(iss, line,'\n');
        v.push_back(line);
    }
    return std::move(v);
}

std::optional<SRunContainer> SMysqlInterface::getRunContainer(long runid) {
   cpr::Response r = cpr::Get(cpr::Url{std::string(this->api_url) + "retrieve/validity/latest/" + std::to_string(runid)},
                            cpr::Header{{"Authorization", "Token " + std::string(this->auth_token)}});
    
    if( r.status_code == 200){
        const char* json = r.text.c_str();
        Document d;
        d.Parse(json);
        if(d.HasMember("data")){
            SRunContainer run_container;
            const Value& data = d["data"];
            std::time_t valid_from = extract_date(
                std::string(data["valid_from"].GetString(), data["valid_from"].GetStringLength()));
            std::time_t valid_to = extract_date(
                std::string(data["valid_to"].GetString(), data["valid_to"].GetStringLength()));
            run_container.setId(runid);
            run_container.setStart(valid_from);
            run_container.setStop(valid_to);
            return { std::move(run_container) };
        }
    }
    return std::nullopt;
}

/**
 * Return run containers from given run id range.
 * \param rinid_min minimal runid, weak ordering
 * \param runid_max maximal runid, strong ordering
 * \return array of objects, empty if no matches
 */
TObjArray SMysqlInterface::getRunContainers(long runid_min, long runid_max)
{ 
    cpr::Response r = cpr::Get(cpr::Url{std::string(this->api_url) + "retrieve/validity/latest/" + std::to_string(runid_min) + "/" + std::to_string(runid_max)},
                               cpr::Header{{"Authorization", "Token " + std::string(this->auth_token)}});
    TObjArray array;
    if( r.status_code == 200){
        const char* json = r.text.c_str();
        Document d;
        d.Parse(json);
        if(d.HasMember("data")){
            const Value& data = d["data"];
            if (data.IsArray()){
                for (auto& v : data.GetArray()){
                    SRunContainer* run_container = new SRunContainer;
                    std::time_t valid_from = extract_date(
                        std::string(v["valid_from"].GetString(), v["valid_from"].GetStringLength()));
                    std::time_t valid_to = extract_date(
                        std::string(v["valid_to"].GetString(), v["valid_to"].GetStringLength()));
                    run_container->setStart(valid_from);
                    run_container->setStop(valid_to);
                    array.Add(run_container);
                }
            }
        }
    }
    return array;
}

/**
 * Add new run contaniner. The runid is stored inside the container.
 * \param runcont run container to add.
 */
void SMysqlInterface::addRunContainer(SRunContainer&& runcont) {
    std::time_t valid_from = runcont.getStart();
    std::time_t valid_to = runcont.getStop();
    auto run_id = runcont.getId();

    char valid_from_str[256];
    char valid_to_str[256];
    
    std::strftime(
        valid_from_str, 
        sizeof(valid_from_str), 
        "%Y-%m-%dT%H:%M", 
        gmtime(&valid_from)
    );
    std::strftime(
        valid_to_str, 
        sizeof(valid_to_str), 
        "%Y-%m-%dT%H:%M", 
        gmtime(&valid_to)
    );
    
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

    cpr::Response r = cpr::Post(cpr::Url{std::string(this->api_url) + "runs/"},
                                cpr::Body{strJson},
                                cpr::Header{{"Authorization", "Token " + std::string(this->auth_token)},
                                            {"Content-Type", "application/json"}});
}

std::optional<SContainer> SMysqlInterface::getContainer(std::string_view&& name, long runid)
{
    cpr::Response r = cpr::Get(cpr::Url{std::string(this->api_url) + "retrieve/" + std::string(name) + "/" + std::to_string(runid)},
                               cpr::Header{{"Authorization", "Token " + std::string(this->auth_token)}});
    if( r.status_code == 200){
        const char* json = r.text.c_str();
        Document d;
        d.Parse(json);
        if(d.IsArray()){
            Parameter p;
            int max_version = -1;
            int current_version;
            for (auto& v : d.GetArray()){
                current_version = (int) v["version"].GetInt();
                if (current_version > max_version){
                    max_version = current_version;
                    p.version = max_version;
                    p.param = std::string(v["parameters"].GetString(), v["parameters"].GetStringLength());
                    p.valid_from = std::string(v["valid_from"].GetString(), v["valid_from"].GetStringLength());
                    p.valid_to = std::string(v["valid_to"].GetString(), v["valid_to"].GetStringLength());
                }
            }
            SContainer cont;
            auto validity = validity_range_t(
                extract_date(p.valid_from),
                extract_date(p.valid_to)
            );
            cont.validity = validity;
            cont.lines = split_lines(p.param);
            return { std::move(cont) };
        }
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

    return {};
}

/**
 * Return named containers from given run id range.
 * \param name container name
 * \param rinid_min minimal runid, weak ordering
 * \param runid_max maximal runid, strong ordering
 * \return array of objects, empty if no matches
 */
std::vector<SContainer> SMysqlInterface::getContainers(std::string_view&& name, long runid_min,
                                                       long runid_max)
{
    return {};
}

/**
 * Add new container to the database, by name. It must be later validated via web interface.
 * \return success of the operation
 */
bool SMysqlInterface::addContainer(std::string_view&& name, SContainer&& cont) { return false; }

