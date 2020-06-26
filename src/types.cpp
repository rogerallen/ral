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
#include "types.h"
#include "env.h"

#if 1
#include "aixlog.hpp"
extern bool gDebug;
#define DBG LOG(DEBUG) << COND(gDebug)
#endif

MalTypePtr EVAL(MalTypePtr mp, MalEnvPtr env);

// ================================================================================
// when using a MalType as a key for the MalMap, use this function to 
// get the string to use as a key.  Only String and Keyword types should
// override this and implement it.  Other types should throw an error.
// This avoids checking derived types in the reader function.
std::string MalType::asMapKey() {
    throw MalBadKeyType();
}

// ================================================================================
// most things are not able to apply, only MalFunctions apply.
MalTypePtr MalType::apply(MalTypeIter begin, MalTypeIter end) {
    throw MalNotApplicable();
}

// ================================================================================
// when using MalType for arithmentic, use this function to get the value.
// Only integer will override this an implement it.
int64_t MalType::asInt() {
    throw MalNoIntegerRepresentation();
}

// ================================================================================
// if statement is true if condition is not nil or false
// override this only in the Constant class.
bool MalType::isNilOrFalse() {
    return false;
}

// ================================================================================
MalTypePtr MalType::getMeta() {
    return std::make_shared<MalConstant>("nil");
}

// ================================================================================
void MalType::setMeta(MalTypePtr meta)
{
    throw MalException("Cannot set the meta-data for this type.");
}

// ================================================================================
// overrides for only the list type 
// ???FIXME??? throw Error? -- only when static analysis cannot confirm no issue.
bool MalType::isList() { return false; }
bool MalType::isVector() { return false; }
bool MalType::isEmptyList() { return false; }
MalTypePtr MalType::apply() { return nullptr; }
void MalType::setEnv(MalEnvPtr env) { throw MalBadSetEnv(); }
bool MalType::is_macro_call(MalEnvPtr env) { return false; }

// ================================================================================
MalInteger::MalInteger(std::string s)
{
    repr_ = s;
    value_ = std::stoi(s);
}

MalInteger::MalInteger(int64_t i)
{
    repr_ = std::to_string(i);
    value_ = i;
}

MalInteger::MalInteger(MalInteger *that)
{
    repr_ = that->repr_;
    value_ = that->value_;
}

MalInteger::~MalInteger()
{
}

std::string MalInteger::str(bool readable)
{
    return std::to_string(value_);
}

MalTypePtr MalInteger::eval(MalEnvPtr env)
{
    return std::make_shared<MalInteger>(MalInteger(this));
}

bool MalInteger::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalInteger>(that);
    return value_ == b->asInt();
}

int64_t MalInteger::asInt() {
    return value_;
}

// ================================================================================
MalConstant::MalConstant(std::string s)
{
    repr_ = s;
}

MalConstant::MalConstant(MalConstant *that)
{
    repr_ = that->repr_;
}

MalConstant::~MalConstant()
{
}

std::string MalConstant::str(bool readable)
{
    return repr_;
}

MalTypePtr MalConstant::eval(MalEnvPtr env)
{
    return std::make_shared<MalConstant>(MalConstant(this));
}

bool MalConstant::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalConstant>(that);
    return repr_ == b->str(false);
}

int64_t MalConstant::asInt()
{
    if(repr_ == "nil") {
        return 0;
    }
    else if(repr_ == "true") {
        return 1;
    }
    else { // if(repr_ == "false") {
        return 0;
    }
}

bool MalConstant::isNilOrFalse() {
    return (repr_ == "nil") || (repr_ == "false");
}

// ================================================================================
MalSymbol::MalSymbol(std::string s)
{
    repr_ = s;
}

MalSymbol::~MalSymbol()
{
}

std::string MalSymbol::str(bool readable)
{
    return repr_;
}

MalTypePtr MalSymbol::eval(MalEnvPtr env)
{
    return env->get(repr_);
}

bool MalSymbol::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalSymbol>(that);
    return repr_ == b->str(false);
}


// ================================================================================
// String helpers
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

// transform to printable.  When a string is read, the following transformations 
// are applied: a backslash followed by a doublequote is translated into a plain 
// doublequote character, a backslash followed by "n" is translated into a newline, 
// and a backslash followed by another backslash is translated into a single 
// backslash. 
std::string transformToPrintable(std::string s)
{
    // drop the quotes
    std::string r;
    for(size_t i = 1; i < s.length() - 1; i++) {
        if(s[i] == '\\') {
            if((i + 1 >= s.length() - 1) ||
               !((s[i+1] == '\"') || (s[i+1] == 'n') || (s[i+1] == '\\'))) {
                throw MalUnbalancedBackslash();
            }
            i++;
            switch(s[i]) {
            case '\"':
            case '\\':
                r += s[i];
                break;
            case 'n':
                r += '\n';
                break;
            }
        }
        else {
            r += s[i];
        }
    }
    return r;
}

std::string transformToReadable(std::string s)
{
    std::string r;
    for(size_t i = 0; i < s.length(); i++) {
        switch(s[i]) {
        case '\\':
            r += "\\\\";
            break;
        case '\n':
            r += "\\n";
            break;
        case '\"':
            r += "\\\"";
            break;
        default:
            r += s[i];
            break;
        }
    }
    return r;
}

// ================================================================================
MalString::MalString(std::string s)
{
    repr_ = s;  
}

MalString::MalString(MalString *that)
{
    repr_ = that->repr_;
}

MalString::~MalString()
{
}

std::string MalString::str(bool readable)
{
    return readable ? "\""+transformToReadable(repr_)+"\"" : repr_;
}

MalTypePtr MalString::eval(MalEnvPtr env)
{ 
    return std::make_shared<MalString>(MalString(this));
}

bool MalString::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalString>(that);
    return repr_ == b->str(false);
}

std::string MalString::asMapKey() {
    return str(false);
}

// ================================================================================
MalKeyword::MalKeyword(std::string s)
{
    repr_ = s;
}

MalKeyword::MalKeyword(MalKeyword *that)
{
    repr_ = that->repr_;
}


MalKeyword::~MalKeyword()
{
}

std::string MalKeyword::str(bool readable)
{
    return repr_;
}

MalTypePtr MalKeyword::eval(MalEnvPtr env)
{ 
    return std::make_shared<MalKeyword>(MalKeyword(this));  
}

bool MalKeyword::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalKeyword>(that);
    return repr_ == b->str(false);
}

std::string MalKeyword::asMapKey() {
    return char(255)+str(true);
}

// ================================================================================
MalList::MalList(char listStartChar) 
{
    listStartChar_ = listStartChar;
    meta_ = std::make_shared<MalConstant>("nil");
}

MalList::MalList(std::shared_ptr<MalList> that)
{
    values_ = that->values_;
    listStartChar_ = that->listStartChar_;
    meta_ = that->meta_;
}

MalList::~MalList()
{
}

std::string MalList::listStartStr()
{
    switch(listStartChar_) {
    default:
    case '(':
        return "(";
    case '[':
        return "[";
    }
}

std::string MalList::listEndStr()
{
    switch(listStartChar_) {
    default:
    case '(':
        return ")";
    case '[':
        return "]";
    }
}

std::string MalList::str(bool readable)
{
    std::string s;
    s += listStartStr();
    bool afterFirst = false;
    for (const auto &v : values_) {
        if(afterFirst) {
            s += " ";
        }
        s += v->str(readable);
        afterFirst = true;
    }
    s += listEndStr();
    return s;
}

MalTypePtr MalList::eval(MalEnvPtr env)
{
    // Evaluate all items in the list
    std::vector<MalTypePtr> evaluated;
    for(auto &v: values_) {
        // NOTE EVAL, not v->eval().  This allows for apply()
        evaluated.push_back(EVAL(v,env));
    }
    // return new evaluated list
    auto iter = evaluated.begin();
    MalTypePtr mp = std::make_shared<MalList>(MalList(listStartChar_));
    for(;iter != evaluated.end(); iter++) {
        std::static_pointer_cast<MalList>(mp)->add(*iter);
    }
    return mp;
}

MalTypePtr MalList::get(size_t i)
{
    if(values_.size() > i) {
        return values_[i]; 
    }
    else {
        return std::make_shared<MalConstant>("nil");
    }
}

bool MalList::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalList>(that);
    bool result = false;
    if(size() == b->size()) {
        size_t i = 0; 
        for(;i < values_.size(); i++ ) {
            auto ai = values_[i];
            auto bi = b->get(i);
            if(ai->kind() != bi->kind()) {
                break;
            }
            if(!(ai->equal(bi))) {
                break;
            }
        }
        if(i == values_.size()) {
            result = true;
        }
    }
    return result;
}

// tried to move apply fully into main, but iterators made that troublesome.
MalTypePtr MalList::apply() 
{ 
    // apply the first value as a function
    auto iter = values_.begin();
    auto fn = *iter++;
    auto val = fn->apply(iter,values_.end());
    return val;
}

void MalList::add(MalTypePtr mp)
{
    values_.push_back(mp);
}

MalTypePtr MalList::count()
{
    size_t n = values_.size();
    return std::make_shared<MalInteger>((int64_t)n);
}

bool MalList::isList() { return listStartChar_ == '('; }
bool MalList::isVector() { return listStartChar_ == '['; }
// empty list or vector
bool MalList::isEmptyList() { return values_.size() == 0; }
// setEnv modifies the environment env
void MalList::setEnv(MalEnvPtr env)
{
    if(values_.size() % 2 != 0) {
        throw MalBadSetEnvList();
    }
    // Evaluate all items in the list, updating env
    // NOTE: earlier pairs in the list can affect later pairs
    for(auto iter = values_.begin(); iter != values_.end(); ) {
        auto first = (*iter++)->str(true);
        auto second = (*iter++);
        env->set(first, EVAL(second,env));
    }
}

// This function returns true if ast is a list that contains a symbol 
// as the first element and that symbol refers to a function in the 
// env environment and that function has the is_macro_ attribute set to 
// true. Otherwise, it returns false.
bool MalList::is_macro_call(MalEnvPtr env) 
{ 
    auto first = values_[0];
    if(first->kind() == MalKind::SYMBOL) {
        try {
            auto refers = first->eval(env);
            if(refers->kind() == MalKind::LAMBDA) {
                return std::static_pointer_cast<MalLambda>(refers)->get_is_macro();
            }
        } catch (MalNotInEnvironment e) {
            // if not found, just return false;
            return false;
        }
    }
    return false; 
}

// FIXME DELETE
// handle do special form
MalTypePtr MalList::doList(MalEnvPtr env) 
{ 
    // EVAL() all items in the list, returning the last one
    // instructions say eval_ast, but EVAL is needed
    MalTypePtr mp;
    auto iter = values_.begin();
    iter++; // eat the "do"
    for(; iter != values_.end(); iter++) {
        mp = EVAL(*iter, env);
    }
    return mp; 
}

size_t MalList::size()
{
    return values_.size();
}

MalTypePtr MalList::getMeta() {
    return meta_;
}

void MalList::setMeta(MalTypePtr meta) {
    meta_ = meta;
}

// ================================================================================
MalMap::MalMap() 
{
    meta_ = std::make_shared<MalConstant>("nil");
}

MalMap::MalMap(std::shared_ptr<MalMap> that)
{
    values_ = that->values_;
    meta_ = that->meta_;
}

MalMap::~MalMap()
{
}

std::string MalMap::str(bool readable)
{
    std::string s;
    s += "{";
    bool afterFirst = false;
    for (const auto &k : values_) {
        if(afterFirst) {
            s += " ";
        }
        if((k.first)[0] == char(255)) {
            //DBG << "!" << k.first.substr(1) << "!\n";
            s += k.first.substr(1);
        }
        else {
            //DBG << ">" << k.first << "<\n";
            s += "\""+k.first+"\"";
        }
        s += " ";
        s += k.second->str(readable);
        afterFirst = true;
    }
    s += "}";
    return s;
}

MalTypePtr MalMap::eval(MalEnvPtr env)
{ 
    // Evaluate all values in the map
    std::map<std::string, MalTypePtr> evaluated;
    for(auto &v: values_) {
        // note EVAL (allows for apply())
        evaluated[v.first] = EVAL(v.second, env);
    }
    auto iter = evaluated.begin();
    // return a new evaluated map
    MalTypePtr mp = std::make_shared<MalMap>(MalMap());
    for(;iter != evaluated.end(); iter++) {
        std::static_pointer_cast<MalMap>(mp)->add(iter->first, iter->second);
    }
    return mp;
}

bool MalMap::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalMap>(that);
    bool result = false;
    if(values_.size() == b->values_.size()) {
        auto ai = values_.begin();
        auto bi = b->values_.begin();
        for(; ai != values_.end(); ai++, bi++) {
            auto aik = (*ai).first;
            auto aiv = (*ai).second;
            auto bik = (*bi).first;
            auto biv = (*bi).second;
            if(aik != bik) {
                break;
            }
            if(!(aiv->equal(biv))) {
                break;
            }
        }
        if(ai == values_.end()) {
            result = true;
        }
    }
    return result;
}

void MalMap::add(std::string k, MalTypePtr v)
{
    values_[k] = v;
}

MalTypePtr MalMap::get(MalTypePtr k)
{
    auto key = k->asMapKey();
    auto pos = values_.find(key);
    if(pos == values_.end()) {
        return std::make_shared<MalConstant>("nil");
    }
    return pos->second;
}

void MalMap::remove(MalTypePtr k)
{
    auto key = k->asMapKey();
    DBG << "key = >" << key << "<\n";
    auto pos = values_.find(key);
    if(pos != values_.end()) {
        DBG << "ERASED\n";
        values_.erase(pos);
    }
}

bool MalMap::hasKey(MalTypePtr k)
{
    auto key = k->asMapKey();
    auto pos = values_.find(key);
    return pos != values_.end();
}

MalTypePtr MalMap::getKeys()
{
    auto mp = std::make_shared<MalList>('(');
    for (const auto &p : values_) {
        MalTypePtr mkp;
        std::string key = p.first;
        if((p.first)[0] == char(255)) {
            key = p.first.substr(1);
            mkp = std::make_shared<MalKeyword>(key);
        }
        else {
            mkp = std::make_shared<MalString>(key);
        }
        mp->add(mkp);
    }
    return mp;
}

MalTypePtr MalMap::getVals()
{
    auto mp = std::make_shared<MalList>('(');
    for (const auto &p : values_) {
        mp->add(p.second);
    }
    return mp;
}

MalTypePtr MalMap::getMeta() {
    return meta_;
}

void MalMap::setMeta(MalTypePtr meta) {
    meta_ = meta;
}


// ================================================================================
MalFunction::MalFunction()
{
    name_ = "#<function>:null";
    fn_ = nullptr;
    meta_ = std::make_shared<MalConstant>("nil");
}

MalFunction::MalFunction(std::string name, MalFunctionSignature fn)
{
    name_ = "#<function>:"+name;
    fn_ = fn;
    meta_ = std::make_shared<MalConstant>("nil");
}

MalFunction::~MalFunction()
{}

std::string MalFunction::str(bool readable)
{
    return name_;
}

MalTypePtr MalFunction::eval(MalEnvPtr env)
{ 
    return nullptr; /*FIXME*/ 
}

bool MalFunction::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalFunction>(that);
    return name_ == b->str(false);
}


MalTypePtr MalFunction::apply(MalTypeIter begin, MalTypeIter end)
{
    return (fn_)(begin,end);
}


MalTypePtr MalFunction::getMeta() {
    return meta_;
}

void MalFunction::setMeta(MalTypePtr meta) {
    meta_ = meta;
}

// ================================================================================
MalLambda::MalLambda(std::vector<MalTypePtr> binds, MalTypePtr &form, MalEnvPtr env)
{
    binds_ = binds;
    form_ = form;
    env_ = env;
    is_macro_ = false;
    meta_ = std::make_shared<MalConstant>("nil");
}

MalLambda::MalLambda(std::shared_ptr<MalLambda> that)
{
    binds_ = that->binds_;
    form_ = that->form_;
    env_ = that->env_;
    is_macro_ = that->is_macro_;
    meta_ = that->meta_;
}

MalLambda::~MalLambda() 
{}

std::string MalLambda::str(bool readable)
{
    return "#<function>";
}

MalTypePtr MalLambda::eval(MalEnvPtr env)
{ 
    return nullptr; /*FIXME*/ 
}

bool MalLambda::equal(MalTypePtr that)
{
    auto b = std::static_pointer_cast<MalLambda>(that);
    return false; // FIXME?
}

MalTypePtr MalLambda::apply(MalTypeIter begin, MalTypeIter end)
{
    MalEnvPtr lambda_env = makeEnv(begin, end);
    return EVAL(form_,lambda_env);
}

MalEnvPtr MalLambda::makeEnv(MalTypeIter begin, MalTypeIter end)
{
    std::vector<MalTypePtr> exprs;
    for(auto iter = begin; iter != end; iter++) {
        exprs.push_back(*iter);
    }
    MalEnvPtr lambda_env = std::make_shared<MalEnv>(env_,binds_,exprs); 
    return lambda_env;
}

MalTypePtr MalLambda::getMeta() {
    return meta_;
}

void MalLambda::setMeta(MalTypePtr meta) {
    meta_ = meta;
}

// ================================================================================
MalAtom::MalAtom(MalTypePtr that)
{
    value_ = that;
}
MalAtom::~MalAtom()
{}
std::string MalAtom::str(bool readable)
{
    return "(atom "+value_->str(true)+")";
}
MalTypePtr MalAtom::eval(MalEnvPtr env)
{
    return nullptr; /*FIXME*/ 
}
bool MalAtom::equal(MalTypePtr that)
{
    return false; // FIXME
}
MalTypePtr MalAtom::value()
{
    return value_;
}
MalTypePtr MalAtom::set(MalTypePtr that)
{
    value_ = that;
    return value_;
}