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
#include "malCore.h"
#include "linenoise.hpp"
#include "printer.h"
#include "reader.h"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#if 0
#include "aixlog.hpp"
extern bool gDebug;
#define DBG LOG(DEBUG) << COND(gDebug)
#endif

MalTypePtr apply(MalTypePtr mp);

// ================================================================================
// Setup ns: symbol -> function map
// ================================================================================
const std::map<std::string, MalFunctionSignature> MalCore::ns = {
    {"+", mal_add},
    {"-", mal_sub},
    {"*", mal_mul},
    {"/", mal_div},
    {"list", mal_list},
    {"list?", mal_list_q},
    {"empty?", mal_empty_q},
    {"count", mal_count},
    {"=", mal_equal},
    {"<", mal_lt},
    {"<=", mal_le},
    {">", mal_gt},
    {">=", mal_ge},
    {"pr-str", mal_pr_str},
    {"str", mal_str},
    {"prn", mal_prn},
    {"println", mal_println},
    {"read-string", mal_read_string},
    {"slurp", mal_slurp},
    {"atom", mal_atom},
    {"atom?", mal_atom_q},
    {"deref", mal_deref},
    {"reset!", mal_reset},
    {"swap!", mal_swap},
    {"cons", mal_cons},
    {"concat", mal_concat},
    {"nth", mal_nth},
    {"first", mal_first},
    {"rest", mal_rest},
    {"throw", mal_throw},
    {"apply", mal_apply},
    {"map", mal_map},
    {"nil?", mal_nil_q},
    {"true?", mal_true_q},
    {"false?", mal_false_q},
    {"symbol?", mal_symbol_q},
    {"symbol", mal_symbol},
    {"keyword", mal_keyword},
    {"keyword?", mal_keyword_q},
    {"vector", mal_vector},
    {"vector?", mal_vector_q},
    {"sequential?", mal_sequential_q},
    {"hash-map", mal_hash_map},
    {"map?", mal_map_q},
    {"assoc", mal_assoc},
    {"dissoc", mal_dissoc},
    {"get", mal_get},
    {"contains?", mal_contains_q},
    {"keys", mal_keys},
    {"vals", mal_vals},
    {"readline", mal_readline},
    {"time-ms", mal_time_ms},
    {"meta", mal_meta},
    {"with-meta", mal_with_meta},
    {"fn?", mal_fn_q},
    {"string?", mal_string_q},
    {"number?", mal_number_q},
    {"seq", mal_seq},
    {"conj", mal_conj},
    {"macro?", mal_macro_q}
};

// ================================================================================
// CHECKS
// ================================================================================

void checkArgsEqual(const char *name, size_t expected, size_t num)
{
    if (expected != num) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires " << expected << " parameters and " << num << " were provided.";
        throw MalException(errSS.str());
    }
}
#if 0
void checkArgsBetween(const char* name, size_t min, size_t max, size_t num) 
{
    if((num < min) || (num > max)) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires between " << min << " and " << max << " parameters and " << 
            num << " were provided."; 
        throw MalException(errSS.str());
    }
}
#endif
void checkArgsAtLeast(const char *name, size_t min, size_t num)
{
    if (num < min) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires at least " << min << " parameters and " << num << " were provided.";
        throw MalException(errSS.str());
    }
}
void checkArgsEven(const char *name, size_t num)
{
    if (num % 2 != 0) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires an even number of parameters and " << num << " were provided.";
        throw MalException(errSS.str());
    }
}
void checkArgsOdd(const char *name, size_t num)
{
    if (num % 2 != 1) {
        std::ostringstream errSS;
        errSS << "'" << name << "' requires an odd number of parameters and " << num << " were provided.";
        throw MalException(errSS.str());
    }
}

// ================================================================================
// Core Functions
// ================================================================================
MalTypePtr mal_add(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("+", 1, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t value = (**(iter++)).asInt();
    for (; iter != end;) {
        value += (**(iter++)).asInt();
    }
    MalTypePtr mp = std::make_shared<MalInteger>(value);
    return mp;
}

// ================================================================================
MalTypePtr mal_sub(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("-", 1, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t value = (**(iter++)).asInt();
    if (iter == end) {
        value = -value;
    }
    else {
        for (; iter != end;) {
            value -= (**(iter++)).asInt();
        }
    }
    MalTypePtr mp = std::make_shared<MalInteger>(value);
    return mp;
}

// ================================================================================
MalTypePtr mal_mul(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("*", 1, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t value = (**(iter++)).asInt();
    for (; iter != end;) {
        value *= (**(iter++)).asInt();
    }
    MalTypePtr mp = std::make_shared<MalInteger>(value);
    return mp;
}

// ================================================================================
MalTypePtr mal_div(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("/", 1, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t value = (**(iter++)).asInt();
    for (; iter != end;) {
        value /= (**(iter++)).asInt();
    }
    MalTypePtr mp = std::make_shared<MalInteger>(value);
    return mp;
}

// ================================================================================
MalTypePtr mal_list(MalTypeIter begin, MalTypeIter end)
{
    MalTypePtr mp = std::make_shared<MalList>('(');
    for (auto iter = begin; iter != end; iter++) {
        std::static_pointer_cast<MalList>(mp)->add(*iter);
    }
    return mp;
}

// ================================================================================
MalTypePtr mal_list_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("list?", 1, std::distance(begin, end));
    return (**begin).isList() ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
MalTypePtr mal_empty_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("empty?", 1, std::distance(begin, end));
    return (**begin).isEmptyList() ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}
// ================================================================================
MalTypePtr mal_count(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("count", 1, std::distance(begin, end));
    if ((**begin).kind() == MalKind::LIST) {
        return std::static_pointer_cast<MalList>(*begin)->count();
    }
    MalTypePtr mp = std::make_shared<MalInteger>(0);
    return mp;
}
// ================================================================================
// =: compare the first two parameters and return true if they are the same type
// and contain the same value. In the case of equal length lists, each element of
// the list should be compared for equality and if they are the same return true, otherwise false.
MalTypePtr mal_equal(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("=", 2, std::distance(begin, end));
    MalTypeIter iter = begin;
    auto a = (*iter++);
    for (; iter != end; iter++) {
        auto b = (*iter);
        if (a->kind() != b->kind()) {
            return std::make_shared<MalConstant>("false");
        }
        if (!(a->equal(b))) {
            return std::make_shared<MalConstant>("false");
        }
    }
    return std::make_shared<MalConstant>("true");
}
// ================================================================================
MalTypePtr mal_lt(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("<", 2, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t a = (*iter++)->asInt();
    int64_t b = (*iter)->asInt();
    return (a < b) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
MalTypePtr mal_le(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("<=", 2, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t a = (*iter++)->asInt();
    int64_t b = (*iter)->asInt();
    return (a <= b) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
MalTypePtr mal_gt(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual(">", 2, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t a = (*iter++)->asInt();
    int64_t b = (*iter)->asInt();
    return (a > b) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
MalTypePtr mal_ge(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual(">=", 2, std::distance(begin, end));
    MalTypeIter iter = begin;
    int64_t a = (*iter++)->asInt();
    int64_t b = (*iter)->asInt();
    return (a >= b) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
// pr-str: calls pr_str on each argument with print_readably set to true, joins the
// results with " " and returns the new string.
MalTypePtr mal_pr_str(MalTypeIter begin, MalTypeIter end)
{
    MalTypeIter iter = begin;
    std::string repr;
    bool first = true;
    while (iter != end) {
        if (!first) {
            repr += " ";
        }
        repr += pr_str(*(iter++), true);
        first = false;
    }
    MalTypePtr mp = std::make_shared<MalString>(repr);
    return mp;
}
// ================================================================================
// str: calls pr_str on each argument with print_readably set to false,
// concatenates the results together ("" separator), and returns the new string.
MalTypePtr mal_str(MalTypeIter begin, MalTypeIter end)
{
    MalTypeIter iter = begin;
    std::string repr;
    while (iter != end) {
        repr += pr_str(*(iter++), false);
    }
    MalTypePtr mp = std::make_shared<MalString>(repr);
    return mp;
}
// ================================================================================
// prn: calls pr_str on each argument with print_readably set to true, joins the
// results with " ", prints the string to the screen and then returns nil.
MalTypePtr mal_prn(MalTypeIter begin, MalTypeIter end)
{
    MalTypeIter iter = begin;
    bool first = true;
    while (iter != end) {
        if (!first) {
            std::cout << " ";
        }
        std::cout << pr_str(*(iter++), true);
        first = false;
    }
    std::cout << "\n";
    return std::make_shared<MalConstant>("nil");
}

// ================================================================================
// println: calls pr_str on each argument with print_readably set to false, joins
// the results with " ", prints the string to the screen and then returns nil.
MalTypePtr mal_println(MalTypeIter begin, MalTypeIter end)
{
    MalTypeIter iter = begin;
    bool first = true;
    while (iter != end) {
        if (!first) {
            std::cout << " ";
        }
        std::cout << pr_str(*(iter++), false);
        first = false;
    }
    std::cout << "\n";
    return std::make_shared<MalConstant>("nil");
}

// ================================================================================
// this function just exposes the read_str function from the reader.
MalTypePtr mal_read_string(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("read-string", 1, std::distance(begin, end));
    std::string s = (*begin)->str(false);
    return read_str(s);
}

// ================================================================================
// slurp: this function takes a file name (string) and returns the contents of the file as a string
MalTypePtr mal_slurp(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("slurp", 1, std::distance(begin, end));
    std::string filename = (*begin)->str(false);
    // https://stackoverflow.com/questions/524591/performance-of-creating-a-c-stdstring-from-an-input-iterator/524843#524843
    std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(fileSize);
    ifs.read(&bytes[0], fileSize);
    std::string s = std::string(&bytes[0], fileSize);
    return std::make_shared<MalString>(s);
}

// ================================================================================
MalTypePtr mal_atom(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("atom", 1, std::distance(begin, end));
    return std::make_shared<MalAtom>(*begin);
}

// ================================================================================
MalTypePtr mal_atom_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("atom?", 1, std::distance(begin, end));
    bool value = (*begin)->kind() == MalKind::ATOM;
    std::string s = value ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
MalTypePtr mal_deref(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("deref", 1, std::distance(begin, end));
    return std::static_pointer_cast<MalAtom>(*begin)->value();
}

// ================================================================================
MalTypePtr mal_reset(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("reset", 2, std::distance(begin, end));
    auto iter = begin;
    auto a = *(iter++);
    auto b = *(iter++);
    return std::static_pointer_cast<MalAtom>(a)->set(b);
}

// ================================================================================
// swap!: Takes an atom, a function, and zero or more function arguments. The
// atom's value is modified to the result of applying the function with the atom's
// value as the first argument and the optionally given function arguments as the
// rest of the arguments. The new atom's value is returned.
MalTypePtr mal_swap(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("swap!", 2, std::distance(begin, end));
    auto iter = begin;
    auto atom = *(iter++);
    auto atom_val = std::static_pointer_cast<MalAtom>(atom)->value();
    auto fn = *(iter++);
    // create a list to apply
    auto fnp = std::make_shared<MalList>('(');
    std::static_pointer_cast<MalList>(fnp)->add(fn);
    std::static_pointer_cast<MalList>(fnp)->add(atom_val);
    for (; iter != end; iter++) {
        std::static_pointer_cast<MalList>(fnp)->add(*iter);
    }
    MalTypePtr result = apply(fnp);
    return std::static_pointer_cast<MalAtom>(atom)->set(result);
}

// ================================================================================
// cons: this function takes a list as its second parameter and returns a new list
// that has the first argument prepended to it.
MalTypePtr mal_cons(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("cons", 2, std::distance(begin, end));
    auto iter = begin;
    auto first = *(iter++);
    auto list = *(iter++);
    auto cons = std::make_shared<MalList>('(');
    std::static_pointer_cast<MalList>(cons)->add(first);
    size_t size = std::static_pointer_cast<MalList>(list)->size();
    for (size_t i = 0; i < size; i++) {
        auto item = std::static_pointer_cast<MalList>(list)->get(i);
        std::static_pointer_cast<MalList>(cons)->add(item);
    }
    return cons;
}
// ================================================================================
// concat: this functions takes 0 or more lists as parameters and returns a new
// list that is a concatenation of all the list parameters.
MalTypePtr mal_concat(MalTypeIter begin, MalTypeIter end)
{
    auto list = std::make_shared<MalList>('(');
    for (auto iter = begin; iter != end; iter++) {
        auto listparam = std::static_pointer_cast<MalList>(*iter);
        for (size_t i = 0; i < listparam->size(); i++) {
            auto item = listparam->get(i);
            std::static_pointer_cast<MalList>(list)->add(item);
        }
    }
    return list;
}

// ================================================================================
// nth: this function takes a list (or vector) and a number (index) as arguments,
// returns the element of the list at the given index. If the index is out of
// range, this function raises an exception.
MalTypePtr mal_nth(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("nth", 2, std::distance(begin, end));
    auto iter = begin;
    auto list = *(iter++);
    auto arg = *(iter++);
    if (list->kind() == MalKind::LIST) {
        auto index = arg->asInt();
        if (index >= (int)std::static_pointer_cast<MalList>(list)->size()) {
            throw MalIndexOutOfRange();
        }
        return std::static_pointer_cast<MalList>(list)->get(index);
    }
    throw MalIndexOutOfRange();
}

// ================================================================================
// first: this function takes a list (or vector) as its argument and return the
// first element. If the list (or vector) is empty or is nil then nil is returned.
MalTypePtr mal_first(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("first", 1, std::distance(begin, end));
    auto arg = *begin;
    if (arg->kind() == MalKind::LIST) {
        return std::static_pointer_cast<MalList>(arg)->get(0);
    }
    return std::make_shared<MalConstant>("nil");
}

// ================================================================================
// rest: this function takes a list (or vector) as its argument and returns a
// new list containing all the elements except the first. If the list (or vector)
// is empty or is nil then () (empty list) is returned.
MalTypePtr mal_rest(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("rest", 1, std::distance(begin, end));
    auto arg = *begin;
    auto rest = std::make_shared<MalList>('(');
    if (arg->kind() == MalKind::LIST) {
        size_t size = std::static_pointer_cast<MalList>(arg)->size();
        for (size_t i = 1; i < size; i++) {
            auto item = std::static_pointer_cast<MalList>(arg)->get(i);
            std::static_pointer_cast<MalList>(rest)->add(item);
        }
    }
    return rest;
}

// ================================================================================
// throw: this function takes a mal type/value and throws/raises it as an exception.
MalTypePtr mal_throw(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("throw", 1, std::distance(begin, end));
    throw MalException((*begin)->str(false));
    return nullptr;
}

// ================================================================================

// apply: takes at least two arguments. The first argument is a function and the
// last argument is list (or vector). The arguments between the function and the
// last argument (if there are any) are concatenated with the final argument to
// create the arguments that are used to call the function. The apply function
// allows a function to be called with arguments that are contained in a list
// (or vector). In other words, (apply F A B [C D]) is equivalent to (F A B C D).
MalTypePtr mal_apply(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("apply", 2, std::distance(begin, end));
    auto iter = begin;
    auto fn_list = std::make_shared<MalList>('(');
    // add the fn
    std::static_pointer_cast<MalList>(fn_list)->add(*iter++);
    // add any params in the middle
    long middle_size = end - begin - 2;
    if (middle_size > 0) {
        // create one list from items + final list
        for (long i = 0; i < middle_size; i++) {
            std::static_pointer_cast<MalList>(fn_list)->add(*iter++);
        }
    }
    // add params in the final list
    auto last = *iter++;
    for (size_t i = 0; i < std::static_pointer_cast<MalList>(last)->size(); i++) {
        auto item = std::static_pointer_cast<MalList>(last)->get(i);
        std::static_pointer_cast<MalList>(fn_list)->add(item);
    }
    // run that function
    return apply(fn_list);
}

// ================================================================================
// map: takes a function and a list (or vector) and evaluates the function
// against every element of the list (or vector) one at a time and returns
// the results as a list.
MalTypePtr mal_map(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("map", 2, std::distance(begin, end));
    auto iter = begin;
    auto fn = *iter++;
    auto list = *iter++;
    auto result = std::make_shared<MalList>('(');
    auto num_items = std::static_pointer_cast<MalList>(list)->size();
    for (size_t i = 0; i < num_items; i++) {
        auto fn_list = std::make_shared<MalList>('(');
        std::static_pointer_cast<MalList>(fn_list)->add(fn);
        auto item = std::static_pointer_cast<MalList>(list)->get(i);
        std::static_pointer_cast<MalList>(fn_list)->add(item);
        std::static_pointer_cast<MalList>(result)->add(apply(fn_list));
    }
    return result;
}

// ================================================================================
//
MalTypePtr mal_nil_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("nil?", 1, std::distance(begin, end));
    bool is_const = (*begin)->kind() == MalKind::CONSTANT;
    std::string s = (is_const && ((*begin)->str(true) == "nil")) ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
//
MalTypePtr mal_true_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("true?", 1, std::distance(begin, end));
    bool is_const = (*begin)->kind() == MalKind::CONSTANT;
    std::string s = (is_const && ((*begin)->str(true) == "true")) ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
//
MalTypePtr mal_false_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("false?", 1, std::distance(begin, end));
    bool is_const = (*begin)->kind() == MalKind::CONSTANT;
    std::string s = (is_const && ((*begin)->str(true) == "false")) ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
//
MalTypePtr mal_symbol_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("symbol?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == MalKind::SYMBOL ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
// symbol: takes a string and returns a new symbol with the string as its name.
MalTypePtr mal_symbol(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("symbol", 1, std::distance(begin, end));
    auto str = (*begin)->str(false);
    return std::make_shared<MalSymbol>(str);
}

// ================================================================================
// keyword: takes a string and returns a keyword with the same name (usually just
// be prepending the special keyword unicode symbol). This function should also
// detect if the argument is already a keyword and just return it.
MalTypePtr mal_keyword(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("keyword", 1, std::distance(begin, end));
    auto mp = (*begin);
    if (mp->kind() == MalKind::KEYWORD) {
        return mp;
    }
    auto str = ":" + (*begin)->str(false);
    return std::make_shared<MalKeyword>(str);
}

// ================================================================================
// keyword?: takes a single argument and returns true (mal true value) if the
// argument is a keyword, otherwise returns false (mal false value).
MalTypePtr mal_keyword_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("keyword?", 1, std::distance(begin, end));
    return ((*begin)->kind() == MalKind::KEYWORD) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
// vector: takes a variable number of arguments and returns a vector containing
// those arguments.
MalTypePtr mal_vector(MalTypeIter begin, MalTypeIter end)
{
    MalTypePtr mp = std::make_shared<MalList>('[');
    for (auto iter = begin; iter != end; iter++) {
        std::static_pointer_cast<MalList>(mp)->add(*iter);
    }
    return mp;
}

// ================================================================================
// vector?: takes a single argument and returns true (mal true value) if the
// argument is a vector, otherwise returns false (mal false value).
MalTypePtr mal_vector_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("vector?", 1, std::distance(begin, end));
    return ((*begin)->isVector()) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
// sequential?: takes a single argument and returns true (mal true value) if it
// is a list or a vector, otherwise returns false (mal false value).
MalTypePtr mal_sequential_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("sequential?", 1, std::distance(begin, end));
    return ((*begin)->isList() || (*begin)->isVector()) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
// hash-map: takes a variable but even number of arguments and returns a new mal
// hash-map value with keys from the odd arguments and values from the even
// arguments respectively. This is basically the functional form of the {}
// reader literal syntax.
MalTypePtr mal_hash_map(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEven("hash-map", std::distance(begin, end));
    auto mp = std::make_shared<MalMap>();
    for (auto iter = begin; iter != end; iter++) {
        auto keyp = *iter++;
        auto valuep = *iter;
        std::string key = keyp->asMapKey();
        std::static_pointer_cast<MalMap>(mp)->add(key, valuep);
    }
    return mp;
}

// ================================================================================
// map?: takes a single argument and returns true (mal true value) if the argument is a hash-map, otherwise returns false (mal false value).
MalTypePtr mal_map_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("map?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == MalKind::MAP ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
// assoc: takes a hash-map as the first argument and the remaining arguments are
// odd/even key/value pairs to "associate" (merge) into the hash-map. Note that
// the original hash-map is unchanged (remember, mal values are immutable), and a
// new hash-map containing the old hash-maps key/values plus the merged key/value
// arguments is returned.
MalTypePtr mal_assoc(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("assoc", 3, std::distance(begin, end));
    checkArgsOdd("assoc", std::distance(begin, end));
    auto iter = begin;
    auto mp = std::make_shared<MalMap>(std::static_pointer_cast<MalMap>(*iter++));
    for (; iter != end; iter++) {
        auto keyp = *iter++;
        auto valuep = *iter;
        std::string key = keyp->asMapKey();
        std::static_pointer_cast<MalMap>(mp)->add(key, valuep);
    }
    return mp;
}

// ================================================================================
// dissoc: takes a hash-map and a list of keys to remove from the hash-map.
// Again, note that the original hash-map is unchanged and a new hash-map with
// the keys removed is returned. Key arguments that do not exist in the hash-map
// are ignored.
MalTypePtr mal_dissoc(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("dissoc", 2, std::distance(begin, end));
    auto iter = begin;
    auto mp = std::make_shared<MalMap>(std::static_pointer_cast<MalMap>(*iter++));
    for (; iter != end; iter++) {
        std::static_pointer_cast<MalMap>(mp)->remove(*iter);
    }
    return mp;
}

// ================================================================================
// get: takes a hash-map and a key and returns the value of looking up that key
// in the hash-map. If the key is not found in the hash-map then nil is returned.
MalTypePtr mal_get(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("get", 2, std::distance(begin, end));
    auto iter = begin;
    auto hash_map = *iter++;
    auto key = *iter++;
    if (hash_map->kind() == MalKind::MAP) {
        return std::static_pointer_cast<MalMap>(hash_map)->get(key);
    }
    else {
        return std::make_shared<MalConstant>("nil");
    }
}

// ================================================================================
// contains?: takes a hash-map and a key and returns true (mal true value) if
// the key exists in the hash-map and false (mal false value) otherwise.
MalTypePtr mal_contains_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("contains?", 2, std::distance(begin, end));
    auto iter = begin;
    auto hashmap = *iter++;
    auto key = *iter;
    return std::static_pointer_cast<MalMap>(hashmap)->hasKey(key) ? std::make_shared<MalConstant>("true") : std::make_shared<MalConstant>("false");
}

// ================================================================================
// keys: takes a hash-map and returns a list (mal list value) of all the keys
// in the hash-map.
MalTypePtr mal_keys(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("keys", 1, std::distance(begin, end));
    return std::static_pointer_cast<MalMap>(*begin)->getKeys();
}

// ================================================================================
// vals: takes a hash-map and returns a list (mal list value) of all the values
// in the hash-map.
MalTypePtr mal_vals(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("vals", 1, std::distance(begin, end));
    return std::static_pointer_cast<MalMap>(*begin)->getVals();
}

// ================================================================================
// This functions takes a string that is used to prompt the user for input.
// The line of text entered by the user is returned as a string. If the user
// sends an end-of-file (usually Ctrl-D), then nil is returned.
MalTypePtr mal_readline(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("readline", 1, std::distance(begin, end));
    std::string input;
    auto arg = (*begin);
    std::string argstr = arg->str(false);
    if (linenoise::Readline(argstr.c_str(), input)) {
        // got EOF
        return std::make_shared<MalConstant>("nil");
    }
    return std::make_shared<MalString>(input);
}

// ================================================================================
//
MalTypePtr mal_time_ms(MalTypeIter begin, MalTypeIter end)
{
    
    size_t t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return std::make_shared<MalInteger>(t);
}

// ================================================================================
// meta: this takes a single mal function argument and returns the value of
// the metadata attribute.
MalTypePtr mal_meta(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("meta", 1, std::distance(begin, end));
    return (*begin)->getMeta();
}

// ================================================================================
// with-meta: this function takes two arguments. The first argument is a mal
// function and the second argument is another mal value/type to set as metadata.
// A copy of the mal function is returned that has its meta attribute set to the
// second argument. Note that it is important that the environment and macro
// attribute of mal function are retained when it is copied.
MalTypePtr mal_with_meta(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("with-meta", 2, std::distance(begin, end));
    auto iter = begin;
    auto fn = *iter++;
    auto meta = *iter++;
    MalTypePtr mp;
    switch(fn->kind()) {
    case MalKind::LAMBDA:
        mp = std::make_shared<MalLambda>(std::static_pointer_cast<MalLambda>(fn));
        mp->setMeta(meta);
        break;
    case MalKind::LIST:
        mp = std::make_shared<MalList>(std::static_pointer_cast<MalList>(fn));
        mp->setMeta(meta);
        break;
    case MalKind::MAP:
        mp = std::make_shared<MalMap>(std::static_pointer_cast<MalMap>(fn));
        mp->setMeta(meta);
        break;
    default:
        throw MalException("with-meta not implemented for this type");
    }
    return mp;
}

// ================================================================================
// fn?: returns true if the parameter is a function (internal or user-defined).
MalTypePtr mal_fn_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("fn?", 1, std::distance(begin, end));
    bool condition = false;
    if((*begin)->kind() == MalKind::LAMBDA) {
        condition = !(std::static_pointer_cast<MalLambda>(*begin)->get_is_macro());
    }
    else if((*begin)->kind() == MalKind::FUNCTION) {
        condition = true;
    }
    std::string s = condition ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
//
MalTypePtr mal_string_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("string?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == MalKind::STRING ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
//
MalTypePtr mal_number_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("number?", 1, std::distance(begin, end));
    std::string s = (*begin)->kind() == MalKind::INTEGER ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

// ================================================================================
// seq: takes a list, vector, string, or nil. If an empty list, empty vector, or 
// empty string ("") is passed in then nil is returned. Otherwise, a list is 
// returned unchanged, a vector is converted into a list, and a string is 
// converted to a list that containing the original string split into single 
// character strings.
MalTypePtr mal_seq(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("seq", 1, std::distance(begin, end));
    switch((*begin)->kind()) {
    case MalKind::LIST: {
        auto ml = std::static_pointer_cast<MalList>(*begin);
        if(ml->isEmptyList()) {
            return std::make_shared<MalConstant>("nil");
        }
        else if(ml->isVector()) {
            MalTypePtr mp = std::make_shared<MalList>('(');
            for (size_t i = 0; i < ml->size(); i++) {
                std::static_pointer_cast<MalList>(mp)->add(ml->get(i));
            }
            return mp;
        }
        else {
            return ml; // regular list
        }
    }
    case MalKind::STRING: {
        auto sp = std::static_pointer_cast<MalString>(*begin);
        MalTypePtr mp = std::make_shared<MalList>('(');
        auto str = sp->str(false);
        if(str.size() == 0) {
            return std::make_shared<MalConstant>("nil"); 
        }
        for (auto c: str) {
            auto item = std::make_shared<MalString>(std::string(1,c));
            std::static_pointer_cast<MalList>(mp)->add(item);
        }
        return mp;
    }
    case MalKind::CONSTANT:
        if((*begin)->str(true) == "nil") {
            return std::make_shared<MalConstant>("nil");
        }
    default:
        throw MalException("seq not implemented for this type");
    }
}

// ================================================================================
// conj: takes a collection and one or more elements as arguments and returns 
// a new collection which includes the original collection and the new elements. 
// If the collection is a list, a new list is returned with the elements inserted 
// at the start of the given list in opposite order; if the collection is a vector, 
// a new vector is returned with the elements added to the end of the given vector.
MalTypePtr mal_conj(MalTypeIter begin, MalTypeIter end)
{
    checkArgsAtLeast("seq", 2, std::distance(begin, end));
    auto iter = begin;
    auto first = *iter++;
    switch(first->kind()) {
    case MalKind::LIST: {
        auto ml = std::static_pointer_cast<MalList>(first);
        if(ml->isVector()) {
            MalTypePtr mp = std::make_shared<MalList>('[');
            for (size_t i = 0; i < ml->size(); i++) {
                std::static_pointer_cast<MalList>(mp)->add(ml->get(i));
            }
            for(; iter != end; iter++) {
                std::static_pointer_cast<MalList>(mp)->add(*iter);
            }
            return mp;
        }
        else {
            // List sure is weird.  add params backward to the start
            // of the list...
            MalTypePtr mp = std::make_shared<MalList>('(');
            iter = end;
            iter--;
            for(; iter != begin; iter--) {
                std::static_pointer_cast<MalList>(mp)->add(*iter);
            }
            // then add the original list
            for (size_t i = 0; i < ml->size(); i++) {
                std::static_pointer_cast<MalList>(mp)->add(ml->get(i));
            }
            return mp;
        }
    }
    default:
        throw MalException("conj not implemented for this type");
    }
}

// ================================================================================
//
MalTypePtr mal_macro_q(MalTypeIter begin, MalTypeIter end)
{
    checkArgsEqual("macro?", 1, std::distance(begin, end));
    std::string s = (((*begin)->kind() == MalKind::LAMBDA) && (std::static_pointer_cast<MalLambda>(*begin)->get_is_macro())) ? "true" : "false";
    return std::make_shared<MalConstant>(s);
}

