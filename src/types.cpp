// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
// 
// types.cpp - All of the types handled by ral.  
// INTEGER, CONSTANT, SYMBOL, STRING, KEYWORD,
// LIST, MAP, FUNCTION, LAMBDA, ATOM.
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
#include "types.h"
#include "env.h"

#if 1
#include "aixlog.hpp"
extern bool gDebug;
#define DBG LOG(DEBUG) << COND(gDebug)
#endif

RalTypePtr EVAL(RalTypePtr mp, RalEnvPtr env);

// ================================================================================
// when using a RalType as a key for the RalMap, use this function to 
// get the string to use as a key.  Only String and Keyword types should
// override this and implement it.  Other types should throw an error.
// This avoids checking derived types in the reader function.
std::string RalType::asMapKey() {
    throw RalBadKeyType();
}

// ================================================================================
// most things are not able to apply, only RalFunctions apply.
RalTypePtr RalType::apply(RalTypeIter begin, RalTypeIter end) {
    throw RalNotApplicable();
}

// ================================================================================
// when using RalType for arithmetic, use this function to get the value.
// Only Integer (oops, and Constant) will override this an implement it.
int64_t RalType::asInt() {
    throw RalNoIntegerRepresentation();
}

// ================================================================================
// when using RalType for arithmetic, use this function to get the value.
// Only Double will override this an implement it.
double RalType::asDouble() {
    throw RalNoDoubleRepresentation();
}

// ================================================================================
// if statement is true if condition is not nil or false
// override this only in the Constant class.
bool RalType::isNilOrFalse() {
    return false;
}

// ================================================================================
RalTypePtr RalType::getMeta() {
    return std::make_shared<RalConstant>("nil");
}

// ================================================================================
void RalType::setMeta(RalTypePtr meta)
{
    throw RalException("Cannot set the meta-data for this type.");
}

// ================================================================================
// overrides for only the list type 
// ???FIXME??? throw Error? -- only when static analysis cannot confirm no issue.
bool RalType::isList() { return false; }
bool RalType::isVector() { return false; }
bool RalType::isEmptyList() { return false; }
RalTypePtr RalType::apply() { return nullptr; }
void RalType::setEnv(RalEnvPtr env) { throw RalBadSetEnv(); }
bool RalType::is_macro_call(RalEnvPtr env) { return false; }

// ================================================================================
RalInteger::RalInteger(std::string s)
{
    repr_ = s;
    value_ = std::stoi(s);
    DBG << "***Construct: str " << value_ << " " << this << "\n";
}

RalInteger::RalInteger(int64_t i)
{
    repr_ = std::to_string(i);
    value_ = i;
    DBG << "***Construct: int " << value_ << " " << this << "\n";
}

RalInteger::RalInteger(RalInteger *that)
{
    repr_ = that->repr_;
    value_ = that->value_;
    DBG << "***Construct: copy* " << value_ << " " << this << "\n";
}
/*
RalInteger::RalInteger(RalInteger& that)
{
    repr_ = that.repr_;
    value_ = that.value_;
    DBG << "***Construct: copy& " << value_ << " " << this << "\n";
}
*/

RalInteger::~RalInteger()
{
    DBG << "***Destruct: " << value_ << " " << this << "\n";
}

std::string RalInteger::str(bool readable)
{
    return std::to_string(value_);
}

RalTypePtr RalInteger::eval(RalEnvPtr env)
{
    return shared_from_this();
}

bool RalInteger::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalInteger>(that);
    return value_ == b->asInt();
}

int64_t RalInteger::asInt() {
    return value_;
}

// ================================================================================
RalDouble::RalDouble(std::string s)
{
    repr_ = s;
    value_ = std::stod(s);
    DBG << "***Construct: str " << value_ << " " << this << "\n";
}

RalDouble::RalDouble(double d)
{
    repr_ = std::to_string(d);
    value_ = d;
    DBG << "***Construct: double " << value_ << " " << this << "\n";
}

RalDouble::RalDouble(RalDouble *that)
{
    repr_ = that->repr_;
    value_ = that->value_;
    DBG << "***Construct: copy* " << value_ << " " << this << "\n";
}
/*
RalDouble::RalDouble(RalDouble& that)
{
    repr_ = that.repr_;
    value_ = that.value_;
    DBG << "***Construct: copy& " << value_ << " " << this << "\n";
}
*/

RalDouble::~RalDouble()
{
    DBG << "***Destruct: " << value_ << " " << this << "\n";
}

std::string RalDouble::str(bool readable)
{
    return std::to_string(value_);
}

RalTypePtr RalDouble::eval(RalEnvPtr env)
{
    return shared_from_this();
}

bool RalDouble::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalDouble>(that);
    return value_ == b->asDouble();
}

double RalDouble::asDouble() {
    return value_;
}

// ================================================================================
RalConstant::RalConstant(std::string s)
{
    repr_ = s;
}

RalConstant::RalConstant(RalConstant *that)
{
    repr_ = that->repr_;
}

RalConstant::~RalConstant()
{
}

std::string RalConstant::str(bool readable)
{
    return repr_;
}

RalTypePtr RalConstant::eval(RalEnvPtr env)
{
    return shared_from_this();
}

bool RalConstant::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalConstant>(that);
    return repr_ == b->str(false);
}

int64_t RalConstant::asInt()
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

bool RalConstant::isNilOrFalse() {
    return (repr_ == "nil") || (repr_ == "false");
}

// ================================================================================
RalSymbol::RalSymbol(std::string s)
{
    repr_ = s;
}

RalSymbol::~RalSymbol()
{
}

std::string RalSymbol::str(bool readable)
{
    return repr_;
}

// NOTE: Symbol::eval can return nullptr
// caller (EVAL) needs to deal with this.
RalTypePtr RalSymbol::eval(RalEnvPtr env)
{
    return env->get(repr_);
}

bool RalSymbol::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalSymbol>(that);
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
                throw RalUnbalancedBackslash();
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
RalString::RalString(std::string s)
{
    repr_ = s;  
}

RalString::RalString(RalString *that)
{
    repr_ = that->repr_;
}

RalString::~RalString()
{
}

std::string RalString::str(bool readable)
{
    return readable ? "\""+transformToReadable(repr_)+"\"" : repr_;
}

RalTypePtr RalString::eval(RalEnvPtr env)
{ 
    return shared_from_this();
}

bool RalString::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalString>(that);
    return repr_ == b->str(false);
}

std::string RalString::asMapKey() {
    return str(false);
}

// ================================================================================
RalKeyword::RalKeyword(std::string s)
{
    repr_ = s;
}

RalKeyword::RalKeyword(RalKeyword *that)
{
    repr_ = that->repr_;
}


RalKeyword::~RalKeyword()
{
}

std::string RalKeyword::str(bool readable)
{
    return repr_;
}

RalTypePtr RalKeyword::eval(RalEnvPtr env)
{ 
    return shared_from_this();
}

bool RalKeyword::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalKeyword>(that);
    return repr_ == b->str(false);
}

std::string RalKeyword::asMapKey() {
    return char(255)+str(true);
}

// ================================================================================
RalList::RalList(char listStartChar) 
{
    listStartChar_ = listStartChar;
    meta_ = std::make_shared<RalConstant>("nil");
}

RalList::RalList(std::shared_ptr<RalList> that)
{
    values_ = that->values_;
    listStartChar_ = that->listStartChar_;
    meta_ = that->meta_;
}

RalList::~RalList()
{
}

std::string RalList::listStartStr()
{
    switch(listStartChar_) {
    default:
    case '(':
        return "(";
    case '[':
        return "[";
    }
}

std::string RalList::listEndStr()
{
    switch(listStartChar_) {
    default:
    case '(':
        return ")";
    case '[':
        return "]";
    }
}

std::string RalList::str(bool readable)
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

RalTypePtr RalList::eval(RalEnvPtr env)
{
    // Evaluate all items in the list
    std::vector<RalTypePtr> evaluated;
    for(auto &v: values_) {
        // NOTE EVAL, not v->eval().  This allows for apply()
        evaluated.push_back(EVAL(v,env));
    }
    // return new evaluated list
    auto iter = evaluated.begin();
    RalTypePtr mp = std::make_shared<RalList>(RalList(listStartChar_));
    for(;iter != evaluated.end(); iter++) {
        std::static_pointer_cast<RalList>(mp)->add(*iter);
    }
    return mp;
}

RalTypePtr RalList::get(size_t i)
{
    if(values_.size() > i) {
        return values_[i]; 
    }
    else {
        return std::make_shared<RalConstant>("nil");
    }
}

bool RalList::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalList>(that);
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
RalTypePtr RalList::apply() 
{ 
    // apply the first value as a function
    auto iter = values_.begin();
    auto fn = *iter++;
    auto val = fn->apply(iter,values_.end());
    return val;
}

void RalList::add(RalTypePtr mp)
{
    values_.push_back(mp);
}

RalTypePtr RalList::count()
{
    size_t n = values_.size();
    return std::make_shared<RalInteger>((int64_t)n);
}

bool RalList::isList() { return listStartChar_ == '('; }
bool RalList::isVector() { return listStartChar_ == '['; }
// empty list or vector
bool RalList::isEmptyList() { return values_.size() == 0; }
// setEnv modifies the environment env
void RalList::setEnv(RalEnvPtr env)
{
    if(values_.size() % 2 != 0) {
        throw RalBadSetEnvList();
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
bool RalList::is_macro_call(RalEnvPtr env) 
{ 
    auto first = values_[0];
    if(first->kind() == RalKind::SYMBOL) {
        // SYMBOL eval can be nullptr
        auto refers = first->eval(env);
        if(refers == nullptr) {
            // not found in the environment, just return false
            return false;
        }
        else if(refers->kind() == RalKind::LAMBDA) {
            return std::static_pointer_cast<RalLambda>(refers)->get_is_macro();
        }
    }
    return false; 
}

// FIXME DELETE
// handle do special form
RalTypePtr RalList::doList(RalEnvPtr env) 
{ 
    // EVAL() all items in the list, returning the last one
    // instructions say eval_ast, but EVAL is needed
    RalTypePtr mp;
    auto iter = values_.begin();
    iter++; // eat the "do"
    for(; iter != values_.end(); iter++) {
        mp = EVAL(*iter, env);
    }
    return mp; 
}

size_t RalList::size()
{
    return values_.size();
}

RalTypePtr RalList::getMeta() {
    return meta_;
}

void RalList::setMeta(RalTypePtr meta) {
    meta_ = meta;
}

// ================================================================================
RalMap::RalMap() 
{
    meta_ = std::make_shared<RalConstant>("nil");
}

RalMap::RalMap(std::shared_ptr<RalMap> that)
{
    values_ = that->values_;
    meta_ = that->meta_;
}

RalMap::~RalMap()
{
}

std::string RalMap::str(bool readable)
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

RalTypePtr RalMap::eval(RalEnvPtr env)
{ 
    // Evaluate all values in the map
    std::map<std::string, RalTypePtr> evaluated;
    for(auto &v: values_) {
        // note EVAL (allows for apply())
        evaluated[v.first] = EVAL(v.second, env);
    }
    auto iter = evaluated.begin();
    // return a new evaluated map
    RalTypePtr mp = std::make_shared<RalMap>(RalMap());
    for(;iter != evaluated.end(); iter++) {
        std::static_pointer_cast<RalMap>(mp)->add(iter->first, iter->second);
    }
    return mp;
}

bool RalMap::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalMap>(that);
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

void RalMap::add(std::string k, RalTypePtr v)
{
    values_[k] = v;
}

RalTypePtr RalMap::get(RalTypePtr k)
{
    auto key = k->asMapKey();
    auto pos = values_.find(key);
    if(pos == values_.end()) {
        return std::make_shared<RalConstant>("nil");
    }
    return pos->second;
}

void RalMap::remove(RalTypePtr k)
{
    auto key = k->asMapKey();
    DBG << "key = >" << key << "<\n";
    auto pos = values_.find(key);
    if(pos != values_.end()) {
        DBG << "ERASED\n";
        values_.erase(pos);
    }
}

bool RalMap::hasKey(RalTypePtr k)
{
    auto key = k->asMapKey();
    auto pos = values_.find(key);
    return pos != values_.end();
}

RalTypePtr RalMap::getKeys()
{
    auto mp = std::make_shared<RalList>('(');
    for (const auto &p : values_) {
        RalTypePtr mkp;
        std::string key = p.first;
        if((p.first)[0] == char(255)) {
            key = p.first.substr(1);
            mkp = std::make_shared<RalKeyword>(key);
        }
        else {
            mkp = std::make_shared<RalString>(key);
        }
        mp->add(mkp);
    }
    return mp;
}

RalTypePtr RalMap::getVals()
{
    auto mp = std::make_shared<RalList>('(');
    for (const auto &p : values_) {
        mp->add(p.second);
    }
    return mp;
}

RalTypePtr RalMap::getMeta() {
    return meta_;
}

void RalMap::setMeta(RalTypePtr meta) {
    meta_ = meta;
}


// ================================================================================
RalFunction::RalFunction()
{
    name_ = "#<function>:null";
    fn_ = nullptr;
    meta_ = std::make_shared<RalConstant>("nil");
}

RalFunction::RalFunction(std::shared_ptr<RalFunction> that)
{
    name_ = that->name_;
    fn_ = that->fn_;
    meta_ = that->meta_;
}

RalFunction::RalFunction(std::string name, RalFunctionSignature fn)
{
    name_ = "#<function>:"+name;
    fn_ = fn;
    meta_ = std::make_shared<RalConstant>("nil");
}

RalFunction::~RalFunction()
{}

std::string RalFunction::str(bool readable)
{
    return name_;
}

RalTypePtr RalFunction::eval(RalEnvPtr env)
{ 
    return nullptr; /*FIXME*/ 
}

bool RalFunction::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalFunction>(that);
    return name_ == b->str(false);
}


RalTypePtr RalFunction::apply(RalTypeIter begin, RalTypeIter end)
{
    return (fn_)(begin,end);
}


RalTypePtr RalFunction::getMeta() {
    return meta_;
}

void RalFunction::setMeta(RalTypePtr meta) {
    meta_ = meta;
}

// ================================================================================
RalLambda::RalLambda(std::vector<RalTypePtr> binds, RalTypePtr &form, RalEnvPtr env)
{
    binds_ = binds;
    form_ = form;
    env_ = env;
    is_macro_ = false;
    meta_ = std::make_shared<RalConstant>("nil");
}

RalLambda::RalLambda(RalLambda *that)
{
    binds_ = that->binds_;
    form_ = that->form_;
    env_ = that->env_;
    is_macro_ = that->is_macro_;
    meta_ = that->meta_;
}

RalLambda::RalLambda(std::shared_ptr<RalLambda> that)
{
    binds_ = that->binds_;
    form_ = that->form_;
    env_ = that->env_;
    is_macro_ = that->is_macro_;
    meta_ = that->meta_;
}

RalLambda::~RalLambda() 
{}

std::string RalLambda::str(bool readable)
{
    return "#<function>";
}

RalTypePtr RalLambda::eval(RalEnvPtr env)
{ 
    return nullptr; /*FIXME*/ 
}

bool RalLambda::equal(RalTypePtr that)
{
    auto b = std::static_pointer_cast<RalLambda>(that);
    return false; // FIXME?
}

RalTypePtr RalLambda::apply(RalTypeIter begin, RalTypeIter end)
{
    RalEnvPtr lambda_env = makeEnv(begin, end);
    return EVAL(form_,lambda_env);
}

RalEnvPtr RalLambda::makeEnv(RalTypeIter begin, RalTypeIter end)
{
    std::vector<RalTypePtr> exprs;
    for(auto iter = begin; iter != end; iter++) {
        exprs.push_back(*iter);
    }
    RalEnvPtr lambda_env = std::make_shared<RalEnv>(env_,binds_,exprs); 
    return lambda_env;
}

RalTypePtr RalLambda::getMeta() {
    return meta_;
}

void RalLambda::setMeta(RalTypePtr meta) {
    meta_ = meta;
}

// ================================================================================
RalAtom::RalAtom(RalTypePtr that)
{
    value_ = that;
}
RalAtom::~RalAtom()
{}
std::string RalAtom::str(bool readable)
{
    return "(atom "+value_->str(true)+")";
}
RalTypePtr RalAtom::eval(RalEnvPtr env)
{
    return nullptr; /*FIXME*/ 
}
bool RalAtom::equal(RalTypePtr that)
{
    return false; // FIXME
}
RalTypePtr RalAtom::value()
{
    return value_;
}
RalTypePtr RalAtom::set(RalTypePtr that)
{
    value_ = that;
    return value_;
}