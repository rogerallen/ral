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
#include "reader.h"
#include "aixlog.hpp"
#include <iostream>
#include <regex>

extern bool gDebug;
#define DBG LOG(DEBUG) << COND(gDebug)

// ================================================================================
// Reader Class
// ================================================================================
// Construct Reader with the tokenized list of strings to read.
// The reader maintains state about the current token to process.
Reader::Reader(std::vector<std::string> tokens)
{
    tokens_ = tokens;
    position_ = 0;
}

// ================================================================================
// return the current token and move to the next token.
// returns the empty string if no more tokens are available.
std::string Reader::next()
{
    if (position_ >= tokens_.size()) {
        return ""; // use empty string to signal no more tokens available
    }
    return tokens_[position_++];
}

// ================================================================================
// return the current token only.
// returns the empty string if no more tokens are available.
std::string Reader::peek()
{
    if (position_ >= tokens_.size()) {
        return ""; // use empty string to signal no more tokens available
    }
    return tokens_[position_];
}

// ================================================================================
// Reader associated functions.
// ================================================================================
// read_str will call tokenize and then create a new Reader object instance with
// the tokens. Then it will call read_form with the Reader instance.
RalTypePtr read_str(std::string s)
{
    std::vector<std::string> tokens = tokenize(s);
    Reader r(tokens);
    RalTypePtr mp = read_form(r);
    return mp;
}

// ================================================================================
// read_form will peek at the first token in the Reader object and switch on the
// first character of that token. If the character is a left paren then read_list
// is called with the Reader object. Otherwise, read_atom is called with the Reader
// Object. The return value from read_form is a mal data type.
RalTypePtr read_form(Reader &r)
{
    std::string firstToken = r.peek();
    switch (firstToken[0]) {
    case '(':
    case '[':
    case '{':
        return read_list(r, firstToken[0]);
    default:
        return read_atom(r);
    }
}

// ================================================================================
// read_list will repeatedly call read_form with the Reader object until it
// encounters a ')' token (if it reach EOF before reading a ')' then that is an
// error). It accumulates the results into a List type.
// Now also works for the '[]' vector type and '{}' assoc arrays
RalTypePtr read_list(Reader &r, char listStartChar)
{
    bool listNotMap = listStartChar != '{';
    RalTypePtr mp;
    std::string listEndStr = listStartChar == '(' ? ")" : (listStartChar == '[' ? "]" : "}");
    r.next(); // eat the "(" or "[" or "{" char
    if (listNotMap) {
        mp = std::make_shared<RalList>(listStartChar);
    }
    else {
        mp = std::make_shared<RalMap>();
    }
    DBG << "read_list: start\n";
    while (true) {
        if (r.peek() == "") {
            throw RalMissingParen(); // !!! ERROR !!!  FIXME pass listEndStr
        }
        else if (r.peek() == listEndStr) {
            r.next(); // eat the ")" or "]" or "}"
            break;
        }
        else {
            if (listNotMap) {
                std::static_pointer_cast<RalList>(mp)->add(read_form(r));
            }
            else {
                RalTypePtr keyForm = read_form(r);
                if (r.peek() == "") {
                    throw RalMissingParen(); // !!! ERROR !!!  FIXME pass listEndStr
                }
                else if (r.peek() == listEndStr) {
                    throw RalMissingMapValue();
                }
                else {
                    // this will throw if not good mapkey
                    std::string key = keyForm->asMapKey();
                    RalTypePtr valForm = read_form(r);
                    std::static_pointer_cast<RalMap>(mp)->add(key, valForm);
                }
            }
        }
    }
    DBG << "read_list: end\n";
    return mp;
}

// ================================================================================
// read_atom will look at the contents of the token and return the appropriate
// scalar (simple/single) data type value:
// - numbers (integers)
// - symbols
// - constants: nil, true, false
// - string
// - keyword
// - numbers (float or double) TODO (I want to do this)
// It also handles reader-macro expansion.
static const std::regex integer_regex(R"([+-]\d+|\d+)"); // FIXME +/-, hex, etc.
RalTypePtr read_atom(Reader &r)
{
    std::string repr = r.next();
    if (std::regex_match(repr, integer_regex)) {
        DBG << "read_atom: integer >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalInteger>(repr);
        return mp;
    }
    else if ((repr == "") || (repr == "nil") || (repr == "true") || (repr == "false")) {
        if (repr == "") {
            repr = "nil";
        }
        DBG << "read_atom: constant >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalConstant>(repr);
        return mp;
    }
    else if (repr[0] == '"') {
        if ((repr.length() == 1) || (repr[repr.size() - 1] != '"')) {
            throw RalMissingQuote();
        }
        DBG << "read_atom: string raw>" << repr << "<\n";
        repr = transformToPrintable(repr);
        DBG << "read_atom: string xfm>" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalString>(repr);
        return mp;
    }
    else if (repr[0] == ':') {
        DBG << "read_atom: keyword >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalKeyword>(repr);
        return mp;
    }
    else if (repr == "'") {
        DBG << "read_atom: macro:quote >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(mp)->add(std::make_shared<RalSymbol>("quote"));
        std::static_pointer_cast<RalList>(mp)->add(read_form(r));
        return mp;
    }
    else if (repr == "`") {
        DBG << "read_atom: macro:quasiquote >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(mp)->add(std::make_shared<RalSymbol>("quasiquote"));
        std::static_pointer_cast<RalList>(mp)->add(read_form(r));
        return mp;
    }
    else if (repr == "~") {
        DBG << "read_atom: macro:unquote >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(mp)->add(std::make_shared<RalSymbol>("unquote"));
        std::static_pointer_cast<RalList>(mp)->add(read_form(r));
        return mp;
    }
    else if (repr == "~@") {
        DBG << "read_atom: macro:splice-unquote >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(mp)->add(std::make_shared<RalSymbol>("splice-unquote"));
        std::static_pointer_cast<RalList>(mp)->add(read_form(r));
        return mp;
    }
    else if (repr == "@") {
        DBG << "read_atom: macro:deref >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(mp)->add(std::make_shared<RalSymbol>("deref"));
        std::static_pointer_cast<RalList>(mp)->add(read_form(r));
        return mp;
    }
    // expands the token "^" to return a new list that contains the symbol "with-meta" and 
    // the result of reading the next next form (2nd argument) (read_form) and the next form 
    // (1st argument) in that order (metadata comes first with the ^ macro and the function second).
    else if (repr == "^") {
        DBG << "read_atom: macro:with-meta >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalList>('(');
        std::static_pointer_cast<RalList>(mp)->add(std::make_shared<RalSymbol>("with-meta"));
        auto first = read_form(r);
        auto second = read_form(r);
        std::static_pointer_cast<RalList>(mp)->add(second);
        std::static_pointer_cast<RalList>(mp)->add(first);
        return mp;
    }
    else {
        DBG << "read_atom: symbol >" << repr << "<\n";
        RalTypePtr mp = std::make_shared<RalSymbol>(repr);
        return mp;
    }
}

// ================================================================================
// tokenize will take a single string and return an array of all the tokens.
static const std::regex token_regex(R"([\s,]*(~@|[\[\]{}()'`~^@]|\"(?:\\.|[^\\\"])*\"?|;.*|[^\s\[\]{}('\"`,;)]+))");
std::vector<std::string> tokenize(std::string s)
{
    DBG << "tokenize: >" << s << "<\n";
    std::vector<std::string> tokens;
    std::smatch matches;
    while (std::regex_search(s, matches, token_regex)) {
        auto m = matches[1];
        // save tokens that are not comments
        if (m.str()[0] != ';') {
            tokens.push_back(m);
        }
        s = matches.suffix().str();
    }
    DBG << "tokens:";
    for (auto t : tokens) {
        DBG << " >" << t << "<";
    }
    DBG << "\n";
    return tokens;
}