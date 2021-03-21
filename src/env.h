// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
//
// env.h - environment class
// this class implents holding the data for an environment and a
// pointer to the outer environment.
//
// ======================================================================
// This program is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// ======================================================================
#pragma once

#include "types.h"
#include <map>
#include <string>
#include <vector>

// ================================================================================
// Environment
typedef std::shared_ptr<RalEnv> RalEnvPtr;

class RalEnv : public std::enable_shared_from_this<RalEnv> {
    RalEnvPtr outer_;
    std::map<std::string, RalTypePtr> data_;

  public:
    RalEnv();
    RalEnv(RalEnvPtr outer, std::vector<RalTypePtr> &binds,
           std::vector<RalTypePtr> &exprs);
    void set(std::string name, RalFunctionSignature fn);
    void set(std::string name, RalTypePtr fn);
    RalEnvPtr find(std::string name);
    RalTypePtr get(std::string name);
};