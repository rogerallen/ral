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

#include "malTypes.h"

#include <string>
#include <vector>

class Reader {
    std::vector<std::string> tokens_;
    size_t position_;

  public:
    Reader(std::vector<std::string> tokens);
    std::string next();
    std::string peek();
};

MalTypePtr read_str(std::string s);
MalTypePtr read_form(Reader &r);
MalTypePtr read_list(Reader &r, char listStartChar);
MalTypePtr read_atom(Reader &r);
std::vector<std::string> tokenize(std::string s);