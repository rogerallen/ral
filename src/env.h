// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
// 
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
typedef std::shared_ptr<MalEnv> MalEnvPtr;

class MalEnv : public std::enable_shared_from_this<MalEnv> {
    MalEnvPtr outer_;
    std::map<std::string,MalTypePtr> data_;
public:
    MalEnv();
    MalEnv(MalEnvPtr outer, std::vector<MalTypePtr> &binds, std::vector<MalTypePtr> &exprs);
    void set(std::string name, MalFunctionSignature fn);
    void set(std::string name, MalTypePtr fn);
    MalEnvPtr find(std::string name);
    MalTypePtr get(std::string name);
};