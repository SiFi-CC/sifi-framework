/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef SParDatabaseSource_H
#define SParDatabaseSource_H

#include "sifi_export.h"

#include "SDbAbstractInterface.h"
#include "SParSource.h"
#include "SRun.h"

#include <map>
#include <memory>
#include <string>
#include <variant>

struct SContainer;

namespace SIFI
{

// struct DbInterfaceREST {
//     std::string url;
// };
//
// struct DbInterfaceConnector {
// };

// using db_interface_t = std::variant<DbInterfaceREST,DbInterfaceConnector>;

namespace Auth
{

enum class AuthPermissions
{
    Read,
    Write,
};

struct OnDemandAuth
{
    AuthPermissions perm = AuthPermissions::Write;
};

struct BasicAuth
{
    std::string user;
    std::string password;
    AuthPermissions perm = AuthPermissions::Write;
};

struct TokenAuth
{
    std::string token;
    AuthPermissions perm = AuthPermissions::Read;
};

using auth_variant_t = std::variant<OnDemandAuth, BasicAuth, TokenAuth>;

}; // namespace Auth

class SIFI_EXPORT SParDatabaseSource final : public SParSource
{
public:
    SParDatabaseSource();
    // SParDatabaseSource(db_interface_t db_type, std::string url, std::string user, std::string
    // pass); SParDatabaseSource(db_interface_t db_type, std::string token);
    ~SParDatabaseSource() = default;

    auto authorize() -> bool;
    auto setOpenMode(SourceOpenMode /*mode*/) -> void override;

    auto openRunContainer(int run_type, std::time_t start_time, std::string file_name)
        -> std::optional<SRun>;
    auto closeRunContainer(std::time_t stop_time) -> std::optional<SRun>;

    auto print(std::ostream& os) const -> void override;

private:
    auto doFindContainer(const std::string& name) -> bool override;

    auto doGetContainer(const std::string& name, ulong runid)
        -> std::shared_ptr<::SContainer> override;
    auto doSetContainer(const std::string& name, SContainer&& cont) -> bool;

    auto doInsertContainer(const std::string& name, SContainer* cont) -> bool override;

    auto doInsertContainer(const std::string& name, std::vector<SContainer*> cont) -> bool override;

    auto doGetRuns() -> std::vector<SRun> override;
    auto doGetRun(ulong runid) -> SRun override;
    auto insertRun(SRun run) -> bool override;

    auto doGetExperiment() const -> std::optional<SExperiment> override;

    bool parseSource();

private:
    std::unique_ptr<SDbAbstractInterface> dbcon;
    std::optional<SExperiment> release;
    // db_interface_t interface_type;

    std::variant<Auth::OnDemandAuth, Auth::BasicAuth, Auth::TokenAuth> credentials;
};

SIFI_EXPORT auto make_database_source() -> std::unique_ptr<SParDatabaseSource>;

}; // namespace SIFI

#endif /* SParDatabaseSource_H */
