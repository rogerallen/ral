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
#include "aixlog.hpp"
#include "linenoise.hpp"
#include "core.h"
#include "env.h"
#include "types.h"
#include "printer.h"
#include "reader.h"
#include "version.h"
#include <iostream>
#include <string>
#include <vector>

bool gDebug = false;
#define DBG LOG(DEBUG) << COND(gDebug)

RalTypePtr READ(std::string s);
RalTypePtr EVAL(RalTypePtr mp, RalEnvPtr env);
std::string PRINT(RalTypePtr mp);
RalTypePtr apply(RalTypePtr mp);
std::string rep(std::string s, RalEnvPtr env);
RalTypePtr ral_eval(RalTypeIter begin, RalTypeIter end);
void setup_repl_env(std::vector<std::string> args);
bool is_pair(RalTypePtr lp);
RalTypePtr quasiquote(RalTypePtr mp);
RalTypePtr macroexpand(RalTypePtr ast, RalEnvPtr env);
void completion(const char* editBuffer, std::vector<std::string>& completions);


// made this a global for ral_eval()
// FIXME -- maybe there should be a class that has this and
// also the REPL?  spawn separate threads with different (or shared)
// environments
static RalEnvPtr repl_env = std::make_shared<RalEnv>();

// ================================================================================
// REPL
// ================================================================================
RalTypePtr READ(std::string s)
{
    DBG << s << "\n";
    return read_str(s);
}

// ================================================================================
// EVAL
//   note that mp->eval() is equivalent to eval_ast() in instructions
RalTypePtr EVAL(RalTypePtr mp, RalEnvPtr env)
{
    while (true) {
        // if not list, return eval_ast(ast, env)
        if (!(mp->isList())) {
            DBG << "not list, eval_ast... " << mp->str(true) << "\n";
            return mp->eval(env);
        }
        // if empty[list], return ast
        if (mp->isEmptyList()) {
            DBG << "empty list, return... " << mp->str(true) << "\n";
            return mp;
        }

        mp = macroexpand(mp, env);
        // if not list, return eval_ast(ast, env)
        if (!(mp->isList())) {
            DBG << "(post macroexpand) not list, eval_ast... " << mp->str(true) << "\n";
            return mp->eval(env);
        }

        // since we know at this point 'mp' is a RalList,
        // rely on only list member functions and only use 'lp'
        // from here
        auto lp = std::static_pointer_cast<RalList>(mp);

        // special env functions
        auto first = lp->get(0)->str(true);
        // (def! symbol value) - update env with symbol & EVAL(value)
        if (first == "def!") {
            DBG << "def! " << lp->str(true) << "\n";
            auto symbol = lp->get(1)->str(true);
            auto value = lp->get(2);
            auto e = EVAL(value, env);
            env->set(symbol, e); // update env
            return e;
        }
        // (defmacro! symbol value)
        else if (first == "defmacro!") {
            DBG << "def! " << lp->str(true) << "\n";
            auto symbol = lp->get(1)->str(true);
            auto value = lp->get(2);
            auto e = EVAL(value, env);
            auto lambdap = std::static_pointer_cast<RalLambda>(e);
            lambdap->set_is_macro();
            env->set(symbol, e); // update env
            return e;
        }
        // (macroexpand macro)
        else if (first == "macroexpand") {
            return macroexpand(lp->get(1),env);
        }
        // (let* (sym1 val1 ...) form) - create new letEnv and EVAL(form,letEnv)
        else if (first == "let*") {
            DBG << "let* " << lp->str(true) << "\n";
            auto letEnvList = lp->get(1);
            auto form = lp->get(2);
            std::vector<RalTypePtr> binds;
            std::vector<RalTypePtr> exprs;
            // letEnv is temporary
            RalEnvPtr let_env = std::make_shared<RalEnv>(env, binds, exprs);
            letEnvList->setEnv(let_env);
            //return EVAL(form, let_env);
            // set ast to the last element and loop for tco
            mp = form;
            env = let_env;
        }
        // (do ...)
        else if (first == "do") {
            DBG << "do " << lp->str(true) << "\n";
            //return lp->doList(env);
            // okay, instead of packaging up the items as a list & passing
            // them to eval_ast (which doesn't exist) I'll just do what
            // eval_ast would do which is EVAL each item. (but the last)
            size_t size = lp->size();
            size_t index = 1; // skip the "do"
            RalTypePtr p0, p1;
            for (; index < size - 1; index++) {
                p1 = EVAL(lp->get(index), env);
            }
            // set ast to the last element and loop for tco
            mp = lp->get(index);
        }
        // (if ...)
        else if (first == "if") {
            DBG << "if " << lp->str(true) << "\n";
            auto condition = lp->get(1);
            auto true_form = lp->get(2);
            auto false_form = lp->get(3);
            auto emp = EVAL(condition, env);
            if (!(emp->isNilOrFalse())) {
                //return EVAL(true_form, env);
                mp = true_form; // TCO
            }
            else {
                //return EVAL(false_form, env);
                mp = false_form; // TCO
            }
        }
        // (fn* ...)
        else if (first == "fn*") {
            DBG << "fn* " << lp->str(true) << "\n";
            auto bindings = lp->get(1);
            if (!(bindings->isList() || bindings->isVector())) {
                throw RalBadFnParam1();
            }
            auto bindingList = std::static_pointer_cast<RalList>(bindings);
            auto form = lp->get(2);
            std::vector<RalTypePtr> bindStrs;
            for (size_t i = 0; i < bindingList->size(); i++) {
                bindStrs.push_back(bindingList->get(i));
            }
            return std::make_shared<RalLambda>(bindStrs, form, env);
        }
        // (quote ...)
        else if(first == "quote") {
            DBG << "quote " << lp->str(true) << "\n";
            return lp->get(1);
        }
        // (quasiquote ...)
        else if(first == "quasiquote") {
            DBG << "quasiquote " << lp->str(true) << "\n";
            auto second = lp->get(1);
            mp = quasiquote(second);
        }
        // (try* A (catch* B C))
        // Add a (native language) try/catch block that evaluates A within the 
        // try block and catches all exceptions. If an exception is caught, 
        // then translate it to a ral type/value. For native exceptions this 
        // is either the message string or a ral hash-map that contains the 
        // message string and other attributes of the exception. When a regular 
        // ral type/value is used as an exception, you will probably need to 
        // store it within a native exception type in order to be able to 
        // convey/transport it using the native try/catch mechanism. Then you 
        // will extract the ral type/value from the native exception. Create a 
        // ral environment that binds B to the value of the exception. 
        // Finally, evaluate C using that new environment.
        else if(first == "try*") {
            try {
                auto A = lp->get(1);
                mp = EVAL(A, env);
            }
            catch (std::exception &e) {
                auto ep = std::make_shared<RalString>(e.what());
                if(lp->size() > 2) {
                    auto catchList = lp->get(2);
                    auto clp = std::static_pointer_cast<RalList>(catchList);
                    auto B = clp->get(1);
                    auto C = clp->get(2);
                    std::vector<RalTypePtr> binds;
                    binds.push_back(B);
                    std::vector<RalTypePtr> exprs;
                    exprs.push_back(ep);
                    RalEnvPtr catch_env = std::make_shared<RalEnv>(env, binds, exprs);
                    mp = EVAL(C, catch_env);
                }
                else{
                    mp = ep;
                }
            }
        }
        else {
            DBG << "list: eval,apply... " << lp->str(true) << "\n";
            // evaluate the list & we know this returns a list
            auto elp = std::static_pointer_cast<RalList>(lp->eval(env));
            // check if first element of the list is a LAMBDA
            if (elp->get(0)->kind() == RalKind::LAMBDA) {
                DBG << "lambda apply\n";
                // special case for TCO
                auto lambda = elp->get(0);
                mp = std::static_pointer_cast<RalLambda>(lambda)->form();
                std::vector<RalTypePtr> rest;
                for (size_t i = 1; i < elp->size(); i++) {
                    rest.push_back(elp->get(i));
                }
                env = std::static_pointer_cast<RalLambda>(lambda)->makeEnv(rest.begin(), rest.end());
            }
            else {
                return apply(elp);
            }
        }
    }
}

bool is_pair(RalTypePtr mp)
{
    return (mp->isList() || mp->isVector()) && !(mp->isEmptyList());
}

RalTypePtr quasiquote(RalTypePtr mp)
{
    // if is_pair of ast is false: return a new list containing: 
    // a symbol named "quote" and ast.
    if(!is_pair(mp)) {
        DBG << "!is_pair\n";
        auto list = std::make_shared<RalList>('(');
        auto quote = std::make_shared<RalSymbol>("quote");
        list->add(quote);
        list->add(mp);
        return list;
    }
    // we know mp is a list
    auto lp = std::static_pointer_cast<RalList>(mp);
    auto first = lp->get(0);
    // else if the first element of ast is a symbol named "unquote": 
    // return the second element of ast.
    if (first->str(true) == "unquote") {
        DBG << "unquote\n";
        return lp->get(1);
    }
    // if is_pair of the first element of ast is true and 
    // the first element of first element of ast (ast[0][0]) is a 
    // symbol named "splice-unquote": return a new list containing: 
    // a symbol named "concat", the second element of first element 
    // of ast (ast[0][1]), and the result of calling quasiquote with 
    // the second through last element of ast.
    auto firstlp = std::static_pointer_cast<RalList>(first);
    if(is_pair(first) && firstlp->get(0)->str(true) == "splice-unquote") {
        DBG << "splice-unquote\n";
        auto list = std::make_shared<RalList>('(');
        auto concat = std::make_shared<RalSymbol>("concat");
        list->add(concat);
        list->add(firstlp->get(1));
        auto rest = std::make_shared<RalList>('(');
        for(size_t i = 1; i < lp->size(); i++) {
            rest->add(lp->get(i));
        }
        list->add(quasiquote(rest));
        return list;
    }
    // otherwise: return a new list containing: 
    // a symbol named "cons", 
    // the result of calling quasiquote on first element of ast (ast[0]), 
    // and the result of calling quasiquote with the second through last 
    // element of ast.
    {
        DBG << "else cons\n";
        auto list = std::make_shared<RalList>('(');
        auto cons = std::make_shared<RalSymbol>("cons");
        list->add(cons);
        list->add(quasiquote(first));
        auto rest = std::make_shared<RalList>('(');
        for(size_t i = 1; i < lp->size(); i++) {
            rest->add(lp->get(i));
        }
        list->add(quasiquote(rest));
        return list;
    }
}

// ================================================================================
// This function calls is_macro_call with ast and env and loops 
// while that condition is true. Inside the loop, the first element 
// of the ast list (a symbol), is looked up in the environment to 
// get the macro function. This macro function is then called/applied 
// with the rest of the ast elements (2nd through the last) as arguments. 
// The return value of the macro call becomes the new value of ast. 
// When the loop completes because ast no longer represents a macro 
// call, the current value of ast is returned.
RalTypePtr macroexpand(RalTypePtr ast, RalEnvPtr env) 
{ 
    while(ast->is_macro_call(env)) {
        DBG << "pre: " << ast->str(true) << "\n";
        auto listp = std::static_pointer_cast<RalList>(ast);
        auto symbol = std::static_pointer_cast<RalSymbol>(listp->get(0));
        auto func = std::static_pointer_cast<RalLambda>(symbol->eval(env));
        auto funclist = std::make_shared<RalList>('(');
        funclist->add(func);
        for(size_t i = 1; i < listp->size(); i++) {
            funclist->add(listp->get(i));
        }
        ast = apply(funclist);
        DBG << "post: " << ast->str(true) << "\n";
    }
    return ast;
}

// ================================================================================
std::string PRINT(RalTypePtr mp)
{
    DBG << mp->str(true) << "\n";
    return pr_str(mp, true);
}

// ================================================================================
RalTypePtr apply(RalTypePtr mp) // FIXME -- reconsider based on mp->lp change in eval
{
    DBG << mp->str(true) << "\n";
    // m can only be a RalListType here
    // exposing iterators was too much leaking abstration.
    return mp->apply();
}

// ================================================================================
std::string rep(std::string s, RalEnvPtr env)
{
    return PRINT(EVAL(READ(s), env));
}

// ================================================================================
// FIXME -- move ral_eval into ralCore (eval-with-env form env)?  make eval special form?
RalTypePtr ral_eval(RalTypeIter begin, RalTypeIter end)
{
    return EVAL(*begin, repl_env);
}

// ================================================================================
void setup_repl_env(std::vector<std::string> args)
{
    // setup repl_env core builtins
    for (auto kv : RalCore::ns) {
        repl_env->set(kv.first, kv.second);
    }
    // add eval
    repl_env->set("eval", ral_eval);
    // add commandline arguments
    RalTypePtr argslist = std::make_shared<RalList>('(');
    for (size_t i = 1; i < args.size(); i++) {
        RalTypePtr sp = std::make_shared<RalString>(args[i]);
        std::static_pointer_cast<RalList>(argslist)->add(sp);
    }
    repl_env->set("*ARGV*", argslist);
    repl_env->set("*host-language*", std::make_shared<RalString>("C++"));
    repl_env->set("*version*", std::make_shared<RalString>(RAL_VERSION));
    repl_env->set("*build-type*", std::make_shared<RalString>(RAL_BUILD_TYPE));
    // add some functions
    rep("(def! not (fn* (a) (if a false true)))", repl_env);
    rep("(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", repl_env);
    rep("(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))", repl_env);
}

// this needs more work
// how to find the most-recent interesting first character?
void completion(const char* editBuffer, std::vector<std::string>& completions) 
{
    switch (editBuffer[0]) {
    case 'a':
        completions.push_back("atom");
        completions.push_back("atom?");
        break;
    case 'c':
        completions.push_back("count");
        completions.push_back("cons");
        completions.push_back("concat");
        break;
    case 'd':
        completions.push_back("deref");
        break;
    case 'e':
        completions.push_back("empty?");
        break;
    case 'l':
        completions.push_back("list");
        completions.push_back("list?");
        break;
    case 'p':
        completions.push_back("prn");
        completions.push_back("pr-str");
        completions.push_back("println");
        break;
    case 'r':
        completions.push_back("read-string");
        completions.push_back("reset!");
        break;
    case 's':
        completions.push_back("str");
        completions.push_back("slurp");
        completions.push_back("swap!");
        break;
    }
}

// ================================================================================
int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        // handle flags without passing them
        if (arg == "-v") {
            gDebug = true;
        }
        else {
            args.push_back(arg);
        }
    }
    AixLog::Log::init<AixLog::SinkCerr>(AixLog::Severity::trace, AixLog::Type::normal);
    DBG << "Start\n";

    setup_repl_env(args);
    if (args.size() > 0) {
        DBG << "FILE " << args[0] << "\n";
        try {
            rep("(load-file \"" + args[0] + "\" )", repl_env);
        }
        catch (std::exception &e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
    }
    else {
        DBG << "REPL\n";
        rep("(println (str \"ral v.\" *version* \" \" *build-type*))", repl_env);
        const auto path = "history.txt";
        linenoise::SetCompletionCallback(completion);
        linenoise::SetMultiLine(true);
        linenoise::SetHistoryMaxLen(50);
        linenoise::LoadHistory(path);
        std::string prev_input("");
        while (true) {
            std::string input;
            std::string prompt = prev_input == "" ? "user> " : "..... ";
            // FIXME: how to read a multi-line form?
            if (linenoise::Readline(prompt.c_str(), input)) {
                break;
            }
            try {
                linenoise::AddHistory(input.c_str());
                std::cout << rep(prev_input + input, repl_env) << std::endl;
                prev_input = "";
            }
#pragma warning(suppress : 4101) // unused var 'e'
            catch (RalMissingParen& e) {
                prev_input += input + "\n";
            }
            catch (std::exception &e) {
                std::cerr << "ERROR: " << e.what() << std::endl;
            }
        }
        linenoise::SaveHistory(path);
    }
    DBG << "End\n";
    return 0;
}
