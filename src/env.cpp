// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
//
// env.cpp - environment class
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
#include "env.h"
#include "easylogging++.h"
#include "logging.h"

extern bool gDebug;

// ================================================================================
RalEnv::RalEnv()
{
    outer_ = nullptr;
    // data_ is default initialized
}

RalEnv::RalEnv(RalEnvPtr outer, std::vector<RalTypePtr> &binds,
               std::vector<RalTypePtr> &exprs)
{
    outer_ = outer;
    bool varArgMode = false;
    size_t i = 0;
    for (auto mp : binds) {
        if (!varArgMode) {
            if (mp->str(false) == "&") {
                DBG << "& => varArgMode\n";
                varArgMode = true;
            }
            else {
                DBG << "env_bind: " << mp->str(false) << " = "
                    << exprs[i]->str(true) << "\n";
                data_[mp->str(false)] = exprs[i++];
            }
        }
        else {
            RalTypePtr lp = std::make_shared<RalList>('(');
            while (i < exprs.size()) {
                std::static_pointer_cast<RalList>(lp)->add(exprs[i++]);
            }
            DBG << "env_bind: " << mp->str(false) << " = " << lp->str(true)
                << "\n";
            data_[mp->str(false)] = lp;
        }
    }
}

// ================================================================================
// special setter for functions
void RalEnv::set(const std::string &name, const RalFunctionSignature &fn)
{
    set(name, std::make_shared<RalFunction>(name, fn));
}

// ================================================================================
void RalEnv::set(const std::string &name, const RalTypePtr &mp)
{
    DBG << "env_set: " << name << " = " << mp->str(true) << "\n";
    data_[name] = mp;
}

// ================================================================================
RalEnvPtr RalEnv::find(const std::string &name)
{
    auto it = data_.find(name);
    if (it == data_.end()) {
        // not found
        if (outer_ != nullptr) {
            return outer_->find(name);
        }
        else {
            return nullptr;
        }
    }
    // found
    return shared_from_this();
}
// ================================================================================
// NOTE: env::get used to throw exception when it could not find a symbol.
// now it returns nullptr for a significant speedup.  Callers need to deal
// with potential for nullptr.
RalTypePtr RalEnv::get(const std::string &name)
{
    // using find() for this is harder than just recursive get
    auto it = data_.find(name);
    if (it == data_.end()) {
        // not found
        if (outer_ != nullptr) {
            return outer_->get(name);
        }
        else {
#if 0
            throw RalNotInEnvironment(name);
#else
            return nullptr;
#endif
        }
    }
    return (*it).second;
}
