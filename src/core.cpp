// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
// 
// core.cpp - core functions implemented in C++
// each function implemnents RalFunctionSignature which 
// takes begin & end RalTypeIter and returns RalTypePtr.
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
#include "core.h"
#include "printer.h"
#include "reader.h"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

// Another windows/Visual Studio compile issue
#ifdef __linux__ 
#include "linenoise.hpp"
#else
namespace linenoise  {
    bool Readline(const char*, std::string&);
};
#endif

RalTypePtr apply(RalTypePtr mp);

// ================================================================================
// Setup ns: symbol -> function map
// ================================================================================
const std::map<std::string, RalFunctionSignature> RalCore::ns = {
    {"+", ral_add},
    {"-", ral_sub},
    {"*", ral_mul},
    {"/", ral_div},
    {"sqrt", ral_sqrt_d},
    {"sin", ral_sin_d},
    {"cos", ral_cos_d},
    {"abs", ral_abs},
    {"list", ral_list},
    {"list?", ral_list_q},
    {"empty?", ral_empty_q},
    {"count", ral_count},
    {"=", ral_equal},
    {"<", ral_lt},
    {"<=", ral_le},
    {">", ral_gt},
    {">=", ral_ge},
    {"pr-str", ral_pr_str},
    {"str", ral_str},
    {"prn", ral_prn},
    {"println", ral_println},
    {"read-string", ral_read_string},
    {"slurp", ral_slurp},
    {"atom", ral_atom},
    {"atom?", ral_atom_q},
    {"deref", ral_deref},
    {"reset!", ral_reset},
    {"swap!", ral_swap},
    {"cons", ral_cons},
    {"concat", ral_concat},
    {"nth", ral_nth},
    {"first", ral_first},
    {"rest", ral_rest},
    {"throw", ral_throw},
    {"apply", ral_apply},
    {"map", ral_map},
    {"nil?", ral_nil_q},
    {"true?", ral_true_q},
    {"false?", ral_false_q},
    {"symbol?", ral_symbol_q},
    {"symbol", ral_symbol},
    {"keyword", ral_keyword},
    {"keyword?", ral_keyword_q},
    {"vector", ral_vector},
    {"vector?", ral_vector_q},
    {"sequential?", ral_sequential_q},
    {"hash-map", ral_hash_map},
    {"map?", ral_map_q},
    {"assoc", ral_assoc},
    {"dissoc", ral_dissoc},
    {"get", ral_get},
    {"contains?", ral_contains_q},
    {"keys", ral_keys},
    {"vals", ral_vals},
    {"readline", ral_readline},
    {"time-ms", ral_time_ms},
    {"meta", ral_meta},
    {"with-meta", ral_with_meta},
    {"fn?", ral_fn_q},
    {"string?", ral_string_q},
    {"number?", ral_number_q},
    {"seq", ral_seq},
    {"conj", ral_conj},
    {"macro?", ral_macro_q}
};

// ================================================================================
// CHECKS
// ================================================================================

void checkArgsEqual(const char *name, size_t expected, size_t num)
{
    if (expected != num) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires " << expected << " parameters and " << num << " were provided.";
        throw RalException(errSS.str());
    }
}
void checkArgsAtLeast(const char *name, size_t min, size_t num)
{
    if (num < min) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires at least " << min << " parameters and " << num << " were provided.";
        throw RalException(errSS.str());
    }
}
void checkArgsEven(const char *name, size_t num)
{
    if (num % 2 != 0) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires an even number of parameters and " << num << " were provided.";
        throw RalException(errSS.str());
    }
}
void checkArgsOdd(const char *name, size_t num)
{
    if (num % 2 != 1) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires an odd number of parameters and " << num << " were provided.";
        throw RalException(errSS.str());
    }
}

// ================================================================================
// Core Functions
// ================================================================================
enum arithmetic_type { INTEGER, DOUBLE };
RalTypePtr ral_add(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("+", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if((*iter)->kind() == RalKind::DOUBLE) {
        atype = DOUBLE;
    }
    int64_t i_value;
    double d_value;
    if(atype == DOUBLE) {
        d_value = (**(iter++)).asDouble();
    }
    else {
        i_value = (**(iter++)).asInt();
    }
    for (; iter != end;) {
        if((atype == INTEGER) && (*iter)->kind() == RalKind::DOUBLE) {
            atype = DOUBLE;
            d_value = (double)i_value;
        }
        if(atype == DOUBLE) {
            d_value += (**(iter++)).asDouble();
        }
        else {
            i_value += (**(iter++)).asInt();
        }
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        mp = std::make_shared<RalDouble>(d_value);
    }
    else {
        mp = std::make_shared<RalInteger>(i_value);
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_sub(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("-", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if((*iter)->kind() == RalKind::DOUBLE) {
        atype = DOUBLE;
    }
    int64_t i_value;
    double d_value;
    if(atype == DOUBLE) {
        d_value = (**(iter++)).asDouble();
    }
    else {
        i_value = (**(iter++)).asInt();
    }
    for (; iter != end;) {
        if((atype == INTEGER) && (*iter)->kind() == RalKind::DOUBLE) {
            atype = DOUBLE;
            d_value = (double)i_value;
        }
        if(atype == DOUBLE) {
            d_value -= (**(iter++)).asDouble();
        }
        else {
            i_value -= (**(iter++)).asInt();
        }
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        mp = std::make_shared<RalDouble>(d_value);
    }
    else {
        mp = std::make_shared<RalInteger>(i_value);
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_mul(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("*", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if((*iter)->kind() == RalKind::DOUBLE) {
        atype = DOUBLE;
    }
    int64_t i_value;
    double d_value;
    if(atype == DOUBLE) {
        d_value = (**(iter++)).asDouble();
    }
    else {
        i_value = (**(iter++)).asInt();
    }
    for (; iter != end;) {
        if((atype == INTEGER) && (*iter)->kind() == RalKind::DOUBLE) {
            atype = DOUBLE;
            d_value = (double)i_value;
        }
        if(atype == DOUBLE) {
            d_value *= (**(iter++)).asDouble();
        }
        else {
            i_value *= (**(iter++)).asInt();
        }
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        mp = std::make_shared<RalDouble>(d_value);
    }
    else {
        mp = std::make_shared<RalInteger>(i_value);
    }
    return mp;

}

// ================================================================================
RalTypePtr ral_div(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("/", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if((*iter)->kind() == RalKind::DOUBLE) {
        atype = DOUBLE;
    }
    int64_t i_value;
    double d_value;
    if(atype == DOUBLE) {
        d_value = (**(iter++)).asDouble();
    }
    else {
        i_value = (**(iter++)).asInt();
    }
    for (; iter != end;) {
        if((atype == INTEGER) && (*iter)->kind() == RalKind::DOUBLE) {
            atype = DOUBLE;
            d_value = (double)i_value;
        }
        if(atype == DOUBLE) {
            d_value /= (**(iter++)).asDouble();
        }
        else {
            i_value /= (**(iter++)).asInt();
        }
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        mp = std::make_shared<RalDouble>(d_value);
    }
    else {
        mp = std::make_shared<RalInteger>(i_value);
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_sqrt_d(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("sqrt", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    double value = (**(iter++)).asDouble();
    value = sqrt(value);
    RalTypePtr mp = std::make_shared<RalDouble>(value);
    return mp;
}

// ================================================================================
RalTypePtr ral_sin_d(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("sin", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    double value = (**(iter++)).asDouble();
    value = sin(value);
    RalTypePtr mp = std::make_shared<RalDouble>(value);
    return mp;
}

// ================================================================================
RalTypePtr ral_cos_d(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("cos", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    double value = (**(iter++)).asDouble();
    value = cos(value);
    RalTypePtr mp = std::make_shared<RalDouble>(value);
    return mp;
}

// ================================================================================
RalTypePtr ral_abs(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("abs", 1, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if((*iter)->kind() == RalKind::DOUBLE) {
        atype = DOUBLE;
    }
    int64_t i_value;
    double d_value;
    RalTypePtr mp;
    if(atype == DOUBLE) {
        d_value = (**(iter++)).asDouble();
        d_value = abs(d_value);
        mp = std::make_shared<RalDouble>(d_value);
    }
    else {
        i_value = (**(iter++)).asInt();
        i_value = (i_value < 0) ? -i_value : i_value;
        mp = std::make_shared<RalDouble>((double)i_value);
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_list(RalTypeIter begin, RalTypeIter end)
{
    RalTypePtr mp = std::make_shared<RalList>('(');
    for (auto iter = begin; iter != end; iter++) {
        std::static_pointer_cast<RalList>(mp)->add(*iter);
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_list_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("list?", 1, std::distance(begin, end));
    return (**begin).isList() ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
}

// ================================================================================
RalTypePtr ral_empty_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("empty?", 1, std::distance(begin, end));
    return (**begin).isEmptyList() ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
}
// ================================================================================
RalTypePtr ral_count(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("count", 1, std::distance(begin, end));
    if ((**begin).kind() == RalKind::LIST) {
        return std::static_pointer_cast<RalList>(*begin)->count();
    }
    RalTypePtr mp = std::make_shared<RalInteger>(0);
    return mp;
}
// ================================================================================
// =: compare the first two parameters and return true if they are the same type
// and contain the same value. In the case of equal length lists, each element of
// the list should be compared for equality and if they are the same return true, otherwise false.
RalTypePtr ral_equal(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("=", 2, std::distance(begin, end));
    RalTypeIter iter = begin;
    auto a = (*iter++);
    for (; iter != end; iter++) {
        auto b = (*iter);
        if (a->kind() != b->kind()) {
            return std::make_shared<RalConstant>("false");
        }
        if (!(a->equal(b))) {
            return std::make_shared<RalConstant>("false");
        }
    }
    return std::make_shared<RalConstant>("true");
}
// ================================================================================
RalTypePtr ral_lt(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("<", 2, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if(((**(iter)).kind() == RalKind::DOUBLE) ||
       ((**(iter+1)).kind() == RalKind::DOUBLE)) {
        atype = DOUBLE;
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        double a = (*iter++)->asDouble();
        double b = (*iter)->asDouble();
        mp = (a < b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    else {
        int64_t a = (*iter++)->asInt();
        int64_t b = (*iter)->asInt();
        mp = (a < b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_le(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("<=", 2, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if(((**(iter)).kind() == RalKind::DOUBLE) ||
       ((**(iter+1)).kind() == RalKind::DOUBLE)) {
        atype = DOUBLE;
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        double a = (*iter++)->asDouble();
        double b = (*iter)->asDouble();
        mp = (a <= b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    else {
        int64_t a = (*iter++)->asInt();
        int64_t b = (*iter)->asInt();
        mp = (a <= b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_gt(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual(">", 2, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if(((**(iter)).kind() == RalKind::DOUBLE) ||
       ((**(iter+1)).kind() == RalKind::DOUBLE)) {
        atype = DOUBLE;
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        double a = (*iter++)->asDouble();
        double b = (*iter)->asDouble();
        mp = (a > b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    else {
        int64_t a = (*iter++)->asInt();
        int64_t b = (*iter)->asInt();
        mp = (a > b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    return mp;
}

// ================================================================================
RalTypePtr ral_ge(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual(">=", 2, std::distance(begin, end));
    RalTypeIter iter = begin;
    arithmetic_type atype = INTEGER;
    if(((**(iter)).kind() == RalKind::DOUBLE) ||
       ((**(iter+1)).kind() == RalKind::DOUBLE)) {
        atype = DOUBLE;
    }
    RalTypePtr mp;
    if(atype == DOUBLE) {
        double a = (*iter++)->asDouble();
        double b = (*iter)->asDouble();
        mp = (a >= b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    else {
        int64_t a = (*iter++)->asInt();
        int64_t b = (*iter)->asInt();
        mp = (a >= b) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
    }
    return mp;
}

// ================================================================================
// pr-str: calls pr_str on each argument with print_readably set to true, joins the
// results with " " and returns the new string.
RalTypePtr ral_pr_str(RalTypeIter begin, RalTypeIter end)
{
    RalTypeIter iter = begin;
    std::string repr;
    bool first = true;
    while (iter != end) {
        if (!first) {
            repr += " ";
        }
        repr += pr_str(*(iter++), true);
        first = false;
    }
    RalTypePtr mp = std::make_shared<RalString>(repr);
    return mp;
}
// ================================================================================
// str: calls pr_str on each argument with print_readably set to false,
// concatenates the results together ("" separator), and returns the new string.
RalTypePtr ral_str(RalTypeIter begin, RalTypeIter end)
{
    RalTypeIter iter = begin;
    std::string repr;
    while (iter != end) {
        repr += pr_str(*(iter++), false);
    }
    RalTypePtr mp = std::make_shared<RalString>(repr);
    return mp;
}
// ================================================================================
// prn: calls pr_str on each argument with print_readably set to true, joins the
// results with " ", prints the string to the screen and then returns nil.
RalTypePtr ral_prn(RalTypeIter begin, RalTypeIter end)
{
    RalTypeIter iter = begin;
    bool first = true;
    while (iter != end) {
        if (!first) {
            std::cout << " ";
        }
        std::cout << pr_str(*(iter++), true);
        first = false;
    }
    std::cout << "\n";
    return std::make_shared<RalConstant>("nil");
}

// ================================================================================
// println: calls pr_str on each argument with print_readably set to false, joins
// the results with " ", prints the string to the screen and then returns nil.
RalTypePtr ral_println(RalTypeIter begin, RalTypeIter end)
{
    RalTypeIter iter = begin;
    bool first = true;
    while (iter != end) {
        if (!first) {
            std::cout << " ";
        }
        std::cout << pr_str(*(iter++), false);
        first = false;
    }
    std::cout << "\n";
    return std::make_shared<RalConstant>("nil");
}

// ================================================================================
// this function just exposes the read_str function from the reader.
RalTypePtr ral_read_string(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("read-string", 1, std::distance(begin, end));
    std::string s = (*begin)->str(false);
    return read_str(s);
}

// ================================================================================
// slurp: this function takes a file name (string) and returns the contents of the file as a string
RalTypePtr ral_slurp(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("slurp", 1, std::distance(begin, end));
    std::string filename = (*begin)->str(false);
    // https://stackoverflow.com/questions/524591/performance-of-creating-a-c-stdstring-from-an-input-iterator/524843#524843
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        throw RalException("file not found: " + filename);
    }
    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(fileSize);
    ifs.read(&bytes[0], fileSize);
    std::string s = std::string(&bytes[0], fileSize);
    return std::make_shared<RalString>(s);
}

// ================================================================================
RalTypePtr ral_atom(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("atom", 1, std::distance(begin, end));
    return std::make_shared<RalAtom>(*begin);
}

// ================================================================================
RalTypePtr ral_atom_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("atom?", 1, std::distance(begin, end));
    bool value = (*begin)->kind() == RalKind::ATOM;
    std::string s = value ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
RalTypePtr ral_deref(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("deref", 1, std::distance(begin, end));
    return std::static_pointer_cast<RalAtom>(*begin)->value();
}

// ================================================================================
RalTypePtr ral_reset(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("reset", 2, std::distance(begin, end));
    auto iter = begin;
    auto a = *(iter++);
    auto b = *(iter++);
    return std::static_pointer_cast<RalAtom>(a)->set(b);
}

// ================================================================================
// swap!: Takes an atom, a function, and zero or more function arguments. The
// atom's value is modified to the result of applying the function with the atom's
// value as the first argument and the optionally given function arguments as the
// rest of the arguments. The new atom's value is returned.
RalTypePtr ral_swap(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("swap!", 2, std::distance(begin, end));
    auto iter = begin;
    auto atom = *(iter++);
    auto atom_val = std::static_pointer_cast<RalAtom>(atom)->value();
    auto fn = *(iter++);
    // create a list to apply
    auto fnp = std::make_shared<RalList>('(');
    std::static_pointer_cast<RalList>(fnp)->add(fn);
    std::static_pointer_cast<RalList>(fnp)->add(atom_val);
    for (; iter != end; iter++) {
        std::static_pointer_cast<RalList>(fnp)->add(*iter);
    }
    RalTypePtr result = apply(fnp);
    return std::static_pointer_cast<RalAtom>(atom)->set(result);
}

// ================================================================================
// cons: this function takes a list as its second parameter and returns a new list
// that has the first argument prepended to it.
RalTypePtr ral_cons(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("cons", 2, std::distance(begin, end));
    auto iter = begin;
    auto first = *(iter++);
    auto list = *(iter++);
    auto cons = std::make_shared<RalList>('(');
    std::static_pointer_cast<RalList>(cons)->add(first);
    size_t size = std::static_pointer_cast<RalList>(list)->size();
    for (size_t i = 0; i < size; i++) {
        auto item = std::static_pointer_cast<RalList>(list)->get(i);
        std::static_pointer_cast<RalList>(cons)->add(item);
    }
    return cons;
}
// ================================================================================
// concat: this functions takes 0 or more lists as parameters and returns a new
// list that is a concatenation of all the list parameters.
RalTypePtr ral_concat(RalTypeIter begin, RalTypeIter end)
{
    auto list = std::make_shared<RalList>('(');
    for (auto iter = begin; iter != end; iter++) {
        auto listparam = std::static_pointer_cast<RalList>(*iter);
        for (size_t i = 0; i < listparam->size(); i++) {
            auto item = listparam->get(i);
            std::static_pointer_cast<RalList>(list)->add(item);
        }
    }
    return list;
}

// ================================================================================
// nth: this function takes a list (or vector) and a number (index) as arguments,
// returns the element of the list at the given index. If the index is out of
// range, this function raises an exception.
RalTypePtr ral_nth(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("nth", 2, std::distance(begin, end));
    auto iter = begin;
    auto list = *(iter++);
    auto arg = *(iter++);
    if (list->kind() == RalKind::LIST) {
        auto index = arg->asInt();
        if (index >= (int)std::static_pointer_cast<RalList>(list)->size()) {
            throw RalIndexOutOfRange();
        }
        return std::static_pointer_cast<RalList>(list)->get(index);
    }
    throw RalIndexOutOfRange();
}

// ================================================================================
// first: this function takes a list (or vector) as its argument and return the
// first element. If the list (or vector) is empty or is nil then nil is returned.
RalTypePtr ral_first(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("first", 1, std::distance(begin, end));
    auto arg = *begin;
    if (arg->kind() == RalKind::LIST) {
        return std::static_pointer_cast<RalList>(arg)->get(0);
    }
    return std::make_shared<RalConstant>("nil");
}

// ================================================================================
// rest: this function takes a list (or vector) as its argument and returns a
// new list containing all the elements except the first. If the list (or vector)
// is empty or is nil then () (empty list) is returned.
RalTypePtr ral_rest(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("rest", 1, std::distance(begin, end));
    auto arg = *begin;
    auto rest = std::make_shared<RalList>('(');
    if (arg->kind() == RalKind::LIST) {
        size_t size = std::static_pointer_cast<RalList>(arg)->size();
        for (size_t i = 1; i < size; i++) {
            auto item = std::static_pointer_cast<RalList>(arg)->get(i);
            std::static_pointer_cast<RalList>(rest)->add(item);
        }
    }
    return rest;
}

// ================================================================================
// throw: this function takes a ral type/value and throws/raises it as an exception.
RalTypePtr ral_throw(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("throw", 1, std::distance(begin, end));
    throw RalException((*begin)->str(false));
    return nullptr;
}

// ================================================================================

// apply: takes at least two arguments. The first argument is a function and the
// last argument is list (or vector). The arguments between the function and the
// last argument (if there are any) are concatenated with the final argument to
// create the arguments that are used to call the function. The apply function
// allows a function to be called with arguments that are contained in a list
// (or vector). In other words, (apply F A B [C D]) is equivalent to (F A B C D).
RalTypePtr ral_apply(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("apply", 2, std::distance(begin, end));
    auto iter = begin;
    auto fn_list = std::make_shared<RalList>('(');
    // add the fn
    std::static_pointer_cast<RalList>(fn_list)->add(*iter++);
    // add any params in the middle
    int64_t middle_size = end - begin - 2;
    if (middle_size > 0) {
        // create one list from items + final list
        for (int64_t i = 0; i < middle_size; i++) {
            std::static_pointer_cast<RalList>(fn_list)->add(*iter++);
        }
    }
    // add params in the final list
    auto last = *iter++;
    for (size_t i = 0; i < std::static_pointer_cast<RalList>(last)->size(); i++) {
        auto item = std::static_pointer_cast<RalList>(last)->get(i);
        std::static_pointer_cast<RalList>(fn_list)->add(item);
    }
    // run that function
    return apply(fn_list);
}

// ================================================================================
// map: takes a function and a list (or vector) and evaluates the function
// against every element of the list (or vector) one at a time and returns
// the results as a list.
RalTypePtr ral_map(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("map", 2, std::distance(begin, end));
    auto iter = begin;
    auto fn = *iter++;
    auto list = *iter++;
    auto result = std::make_shared<RalList>('(');
    auto num_items = std::static_pointer_cast<RalList>(list)->size();
    for (size_t i = 0; i < num_items; i++) {
        auto fn_list = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(fn_list)->add(fn);
        auto item = std::static_pointer_cast<RalList>(list)->get(i);
        std::static_pointer_cast<RalList>(fn_list)->add(item);
        std::static_pointer_cast<RalList>(result)->add(apply(fn_list));
    }
    return result;
}

// ================================================================================
//
RalTypePtr ral_nil_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("nil?", 1, std::distance(begin, end));
    bool is_const = (*begin)->kind() == RalKind::CONSTANT;
    std::string s = (is_const && ((*begin)->str(true) == "nil")) ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
//
RalTypePtr ral_true_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("true?", 1, std::distance(begin, end));
    bool is_const = (*begin)->kind() == RalKind::CONSTANT;
    std::string s = (is_const && ((*begin)->str(true) == "true")) ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
//
RalTypePtr ral_false_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("false?", 1, std::distance(begin, end));
    bool is_const = (*begin)->kind() == RalKind::CONSTANT;
    std::string s = (is_const && ((*begin)->str(true) == "false")) ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
//
RalTypePtr ral_symbol_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("symbol?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == RalKind::SYMBOL ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
// symbol: takes a string and returns a new symbol with the string as its name.
RalTypePtr ral_symbol(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("symbol", 1, std::distance(begin, end));
    auto str = (*begin)->str(false);
    return std::make_shared<RalSymbol>(str);
}

// ================================================================================
// keyword: takes a string and returns a keyword with the same name (usually just
// be prepending the special keyword unicode symbol). This function should also
// detect if the argument is already a keyword and just return it.
RalTypePtr ral_keyword(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("keyword", 1, std::distance(begin, end));
    auto mp = (*begin);
    if (mp->kind() == RalKind::KEYWORD) {
        return mp;
    }
    auto str = ":" + (*begin)->str(false);
    return std::make_shared<RalKeyword>(str);
}

// ================================================================================
// keyword?: takes a single argument and returns true (ral true value) if the
// argument is a keyword, otherwise returns false (ral false value).
RalTypePtr ral_keyword_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("keyword?", 1, std::distance(begin, end));
    return ((*begin)->kind() == RalKind::KEYWORD) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
}

// ================================================================================
// vector: takes a variable number of arguments and returns a vector containing
// those arguments.
RalTypePtr ral_vector(RalTypeIter begin, RalTypeIter end)
{
    RalTypePtr mp = std::make_shared<RalList>('[');
    for (auto iter = begin; iter != end; iter++) {
        std::static_pointer_cast<RalList>(mp)->add(*iter);
    }
    return mp;
}

// ================================================================================
// vector?: takes a single argument and returns true (ral true value) if the
// argument is a vector, otherwise returns false (ral false value).
RalTypePtr ral_vector_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("vector?", 1, std::distance(begin, end));
    return ((*begin)->isVector()) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
}

// ================================================================================
// sequential?: takes a single argument and returns true (ral true value) if it
// is a list or a vector, otherwise returns false (ral false value).
RalTypePtr ral_sequential_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("sequential?", 1, std::distance(begin, end));
    return ((*begin)->isList() || (*begin)->isVector()) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
}

// ================================================================================
// hash-map: takes a variable but even number of arguments and returns a new ral
// hash-map value with keys from the odd arguments and values from the even
// arguments respectively. This is basically the functional form of the {}
// reader literal syntax.
RalTypePtr ral_hash_map(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEven("hash-map", std::distance(begin, end));
    auto mp = std::make_shared<RalMap>();
    for (auto iter = begin; iter != end; iter++) {
        auto keyp = *iter++;
        auto valuep = *iter;
        std::string key = keyp->asMapKey();
        std::static_pointer_cast<RalMap>(mp)->add(key, valuep);
    }
    return mp;
}

// ================================================================================
// map?: takes a single argument and returns true (ral true value) if the argument is a hash-map, otherwise returns false (ral false value).
RalTypePtr ral_map_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("map?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == RalKind::MAP ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
// assoc: takes a hash-map as the first argument and the remaining arguments are
// odd/even key/value pairs to "associate" (merge) into the hash-map. Note that
// the original hash-map is unchanged (remember, ral values are immutable), and a
// new hash-map containing the old hash-maps key/values plus the merged key/value
// arguments is returned.
RalTypePtr ral_assoc(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("assoc", 3, std::distance(begin, end));
    checkArgsOdd("assoc", std::distance(begin, end));
    auto iter = begin;
    auto mp = std::make_shared<RalMap>(std::static_pointer_cast<RalMap>(*iter++));
    for (; iter != end; iter++) {
        auto keyp = *iter++;
        auto valuep = *iter;
        std::string key = keyp->asMapKey();
        std::static_pointer_cast<RalMap>(mp)->add(key, valuep);
    }
    return mp;
}

// ================================================================================
// dissoc: takes a hash-map and a list of keys to remove from the hash-map.
// Again, note that the original hash-map is unchanged and a new hash-map with
// the keys removed is returned. Key arguments that do not exist in the hash-map
// are ignored.
RalTypePtr ral_dissoc(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("dissoc", 2, std::distance(begin, end));
    auto iter = begin;
    auto mp = std::make_shared<RalMap>(std::static_pointer_cast<RalMap>(*iter++));
    for (; iter != end; iter++) {
        std::static_pointer_cast<RalMap>(mp)->remove(*iter);
    }
    return mp;
}

// ================================================================================
// get: takes a hash-map and a key and returns the value of looking up that key
// in the hash-map. If the key is not found in the hash-map then nil is returned.
RalTypePtr ral_get(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("get", 2, std::distance(begin, end));
    auto iter = begin;
    auto hash_map = *iter++;
    auto key = *iter++;
    if (hash_map->kind() == RalKind::MAP) {
        return std::static_pointer_cast<RalMap>(hash_map)->get(key);
    }
    else {
        return std::make_shared<RalConstant>("nil");
    }
}

// ================================================================================
// contains?: takes a hash-map and a key and returns true (ral true value) if
// the key exists in the hash-map and false (ral false value) otherwise.
RalTypePtr ral_contains_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("contains?", 2, std::distance(begin, end));
    auto iter = begin;
    auto hashmap = *iter++;
    auto key = *iter;
    return std::static_pointer_cast<RalMap>(hashmap)->hasKey(key) ? std::make_shared<RalConstant>("true") : std::make_shared<RalConstant>("false");
}

// ================================================================================
// keys: takes a hash-map and returns a list (ral list value) of all the keys
// in the hash-map.
RalTypePtr ral_keys(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("keys", 1, std::distance(begin, end));
    return std::static_pointer_cast<RalMap>(*begin)->getKeys();
}

// ================================================================================
// vals: takes a hash-map and returns a list (ral list value) of all the values
// in the hash-map.
RalTypePtr ral_vals(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("vals", 1, std::distance(begin, end));
    return std::static_pointer_cast<RalMap>(*begin)->getVals();
}

// ================================================================================
// This functions takes a string that is used to prompt the user for input.
// The line of text entered by the user is returned as a string. If the user
// sends an end-of-file (usually Ctrl-D), then nil is returned.
RalTypePtr ral_readline(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("readline", 1, std::distance(begin, end));
    std::string input;
    auto arg = (*begin);
    std::string argstr = arg->str(false);
    if (linenoise::Readline(argstr.c_str(), input)) {
        // got EOF
        return std::make_shared<RalConstant>("nil");
    }
    return std::make_shared<RalString>(input);
}

// ================================================================================
//
RalTypePtr ral_time_ms(RalTypeIter begin, RalTypeIter end)
{
    
    size_t t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return std::make_shared<RalInteger>(t);
}

// ================================================================================
// meta: this takes a single ral function argument and returns the value of
// the metadata attribute.
RalTypePtr ral_meta(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("meta", 1, std::distance(begin, end));
    auto iter = begin;
    auto fn = *iter++;
    switch(fn->kind()) {
    case RalKind::FUNCTION:
    case RalKind::LAMBDA:
    case RalKind::LIST:
    case RalKind::MAP:
        return fn->getMeta();
    default:
        throw RalException("meta not implemented for this type");
    }
}

// ================================================================================
// with-meta: this function takes two arguments. The first argument is a ral
// function and the second argument is another ral value/type to set as metadata.
// A copy of the ral function is returned that has its meta attribute set to the
// second argument. Note that it is important that the environment and macro
// attribute of ral function are retained when it is copied.
RalTypePtr ral_with_meta(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("with-meta", 2, std::distance(begin, end));
    auto iter = begin;
    auto fn = *iter++;
    auto meta = *iter++;
    RalTypePtr mp;
    switch(fn->kind()) {
    case RalKind::FUNCTION:
        mp = std::make_shared<RalFunction>(std::static_pointer_cast<RalFunction>(fn));
        mp->setMeta(meta);
        break;
    case RalKind::LAMBDA:
        mp = std::make_shared<RalLambda>(std::static_pointer_cast<RalLambda>(fn));
        mp->setMeta(meta);
        break;
    case RalKind::LIST:
        mp = std::make_shared<RalList>(std::static_pointer_cast<RalList>(fn));
        mp->setMeta(meta);
        break;
    case RalKind::MAP:
        mp = std::make_shared<RalMap>(std::static_pointer_cast<RalMap>(fn));
        mp->setMeta(meta);
        break;
    default:
        throw RalException("with-meta not implemented for this type");
    }
    return mp;
}

// ================================================================================
// fn?: returns true if the parameter is a function (internal or user-defined).
RalTypePtr ral_fn_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("fn?", 1, std::distance(begin, end));
    bool condition = false;
    if((*begin)->kind() == RalKind::LAMBDA) {
        condition = !(std::static_pointer_cast<RalLambda>(*begin)->get_is_macro());
    }
    else if((*begin)->kind() == RalKind::FUNCTION) {
        condition = true;
    }
    std::string s = condition ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
//
RalTypePtr ral_string_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("string?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == RalKind::STRING ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
//
RalTypePtr ral_number_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("number?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == RalKind::INTEGER ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

// ================================================================================
// seq: takes a list, vector, string, or nil. If an empty list, empty vector, or 
// empty string ("") is passed in then nil is returned. Otherwise, a list is 
// returned unchanged, a vector is converted into a list, and a string is 
// converted to a list that containing the original string split into single 
// character strings.
RalTypePtr ral_seq(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("seq", 1, std::distance(begin, end));
    switch((*begin)->kind()) {
    case RalKind::LIST: {
        auto ml = std::static_pointer_cast<RalList>(*begin);
        if(ml->isEmptyList()) {
            return std::make_shared<RalConstant>("nil");
        }
        else if(ml->isVector()) {
            RalTypePtr mp = std::make_shared<RalList>('(');
            for (size_t i = 0; i < ml->size(); i++) {
                std::static_pointer_cast<RalList>(mp)->add(ml->get(i));
            }
            return mp;
        }
        else {
            return ml; // regular list
        }
    }
    case RalKind::STRING: {
        auto sp = std::static_pointer_cast<RalString>(*begin);
        RalTypePtr mp = std::make_shared<RalList>('(');
        auto str = sp->str(false);
        if(str.size() == 0) {
            return std::make_shared<RalConstant>("nil"); 
        }
        for (auto c: str) {
            auto item = std::make_shared<RalString>(std::string(1,c));
            std::static_pointer_cast<RalList>(mp)->add(item);
        }
        return mp;
    }
    case RalKind::CONSTANT:
        if((*begin)->str(true) == "nil") {
            return std::make_shared<RalConstant>("nil");
        }
    default:
        throw RalException("seq not implemented for this type");
    }
}

// ================================================================================
// conj: takes a collection and one or more elements as arguments and returns 
// a new collection which includes the original collection and the new elements. 
// If the collection is a list, a new list is returned with the elements inserted 
// at the start of the given list in opposite order; if the collection is a vector, 
// a new vector is returned with the elements added to the end of the given vector.
RalTypePtr ral_conj(RalTypeIter begin, RalTypeIter end)
{
    checkArgsAtLeast("seq", 2, std::distance(begin, end));
    auto iter = begin;
    auto first = *iter++;
    switch(first->kind()) {
    case RalKind::LIST: {
        auto ml = std::static_pointer_cast<RalList>(first);
        if(ml->isVector()) {
            RalTypePtr mp = std::make_shared<RalList>('[');
            for (size_t i = 0; i < ml->size(); i++) {
                std::static_pointer_cast<RalList>(mp)->add(ml->get(i));
            }
            for(; iter != end; iter++) {
                std::static_pointer_cast<RalList>(mp)->add(*iter);
            }
            return mp;
        }
        else {
            // List sure is weird.  add params backward to the start
            // of the list...
            RalTypePtr mp = std::make_shared<RalList>('(');
            iter = end;
            iter--;
            for(; iter != begin; iter--) {
                std::static_pointer_cast<RalList>(mp)->add(*iter);
            }
            // then add the original list
            for (size_t i = 0; i < ml->size(); i++) {
                std::static_pointer_cast<RalList>(mp)->add(ml->get(i));
            }
            return mp;
        }
    }
    default:
        throw RalException("conj not implemented for this type");
    }
}

// ================================================================================
//
RalTypePtr ral_macro_q(RalTypeIter begin, RalTypeIter end)
{
    checkArgsEqual("macro?", 1, std::distance(begin, end));
    std::string s = (((*begin)->kind() == RalKind::LAMBDA) && (std::static_pointer_cast<RalLambda>(*begin)->get_is_macro())) ? "true" : "false";
    return std::make_shared<RalConstant>(s);
}

