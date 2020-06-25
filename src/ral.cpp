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
#include "malCore.h"
#include "malEnv.h"
#include "malTypes.h"
#include "printer.h"
#include "reader.h"
#include "version.h"
#include <iostream>
#include <string>
#include <vector>

bool gDebug = false;
#define DBG LOG(DEBUG) << COND(gDebug)

MalTypePtr READ(std::string s);
MalTypePtr EVAL(MalTypePtr mp, MalEnvPtr env);
std::string PRINT(MalTypePtr mp);
MalTypePtr apply(MalTypePtr mp);
std::string rep(std::string s, MalEnvPtr env);
MalTypePtr mal_eval(MalTypeIter begin, MalTypeIter end);
void setup_repl_env(std::vector<std::string> args);
bool is_pair(MalTypePtr lp);
MalTypePtr quasiquote(MalTypePtr mp);
MalTypePtr macroexpand(MalTypePtr ast, MalEnvPtr env);
void completion(const char* editBuffer, std::vector<std::string>& completions);


// made this a global for mal_eval()
// FIXME -- maybe there should be a class that has this and
// also the REPL?  spawn separate threads with different (or shared)
// environments
static MalEnvPtr repl_env = std::make_shared<MalEnv>();

// ================================================================================
// REPL
// ================================================================================
MalTypePtr READ(std::string s)
{
    DBG << s << "\n";
    return read_str(s);
}

// ================================================================================
// EVAL
//   note that mp->eval() is equivalent to eval_ast() in instructions
MalTypePtr EVAL(MalTypePtr mp, MalEnvPtr env)
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

        // since we know at this point 'mp' is a MalList,
        // rely on only list member functions and only use 'lp'
        // from here
        auto lp = std::static_pointer_cast<MalList>(mp);

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
            auto lambdap = std::static_pointer_cast<MalLambda>(e);
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
            std::vector<MalTypePtr> binds;
            std::vector<MalTypePtr> exprs;
            // letEnv is temporary
            MalEnvPtr let_env = std::make_shared<MalEnv>(env, binds, exprs);
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
            MalTypePtr p0, p1;
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
                throw MalBadFnParam1();
            }
            auto bindingList = std::static_pointer_cast<MalList>(bindings);
            auto form = lp->get(2);
            std::vector<MalTypePtr> bindStrs;
            for (size_t i = 0; i < bindingList->size(); i++) {
                bindStrs.push_back(bindingList->get(i));
            }
            return std::make_shared<MalLambda>(bindStrs, form, env);
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
        // then translate it to a mal type/value. For native exceptions this 
        // is either the message string or a mal hash-map that contains the 
        // message string and other attributes of the exception. When a regular 
        // mal type/value is used as an exception, you will probably need to 
        // store it within a native exception type in order to be able to 
        // convey/transport it using the native try/catch mechanism. Then you 
        // will extract the mal type/value from the native exception. Create a 
        // mal environment that binds B to the value of the exception. 
        // Finally, evaluate C using that new environment.
        else if(first == "try*") {
            try {
                auto A = lp->get(1);
                mp = EVAL(A, env);
            }
            catch (std::exception &e) {
                auto ep = std::make_shared<MalString>(e.what());
                if(lp->size() > 2) {
                    auto catchList = lp->get(2);
                    auto clp = std::static_pointer_cast<MalList>(catchList);
                    auto B = clp->get(1);
                    auto C = clp->get(2);
                    std::vector<MalTypePtr> binds;
                    binds.push_back(B);
                    std::vector<MalTypePtr> exprs;
                    exprs.push_back(ep);
                    MalEnvPtr catch_env = std::make_shared<MalEnv>(env, binds, exprs);
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
            auto elp = std::static_pointer_cast<MalList>(lp->eval(env));
            // check if first element of the list is a LAMBDA
            if (elp->get(0)->kind() == MalKind::LAMBDA) {
                DBG << "lambda apply\n";
                // special case for TCO
                auto lambda = elp->get(0);
                mp = std::static_pointer_cast<MalLambda>(lambda)->form();
                std::vector<MalTypePtr> rest;
                for (size_t i = 1; i < elp->size(); i++) {
                    rest.push_back(elp->get(i));
                }
                env = std::static_pointer_cast<MalLambda>(lambda)->makeEnv(rest.begin(), rest.end());
            }
            else {
                return apply(elp);
            }
        }
    }
}

bool is_pair(MalTypePtr mp)
{
    return (mp->isList() || mp->isVector()) && !(mp->isEmptyList());
}

MalTypePtr quasiquote(MalTypePtr mp)
{
    // if is_pair of ast is false: return a new list containing: 
    // a symbol named "quote" and ast.
    if(!is_pair(mp)) {
        DBG << "!is_pair\n";
        auto list = std::make_shared<MalList>('(');
        auto quote = std::make_shared<MalSymbol>("quote");
        list->add(quote);
        list->add(mp);
        return list;
    }
    // we know mp is a list
    auto lp = std::static_pointer_cast<MalList>(mp);
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
    auto firstlp = std::static_pointer_cast<MalList>(first);
    if(is_pair(first) && firstlp->get(0)->str(true) == "splice-unquote") {
        DBG << "splice-unquote\n";
        auto list = std::make_shared<MalList>('(');
        auto concat = std::make_shared<MalSymbol>("concat");
        list->add(concat);
        list->add(firstlp->get(1));
        auto rest = std::make_shared<MalList>('(');
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
        auto list = std::make_shared<MalList>('(');
        auto cons = std::make_shared<MalSymbol>("cons");
        list->add(cons);
        list->add(quasiquote(first));
        auto rest = std::make_shared<MalList>('(');
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
MalTypePtr macroexpand(MalTypePtr ast, MalEnvPtr env) 
{ 
    while(ast->is_macro_call(env)) {
        DBG << "pre: " << ast->str(true) << "\n";
        auto listp = std::static_pointer_cast<MalList>(ast);
        auto symbol = std::static_pointer_cast<MalSymbol>(listp->get(0));
        auto func = std::static_pointer_cast<MalLambda>(symbol->eval(env));
        auto funclist = std::make_shared<MalList>('(');
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
std::string PRINT(MalTypePtr mp)
{
    DBG << mp->str(true) << "\n";
    return pr_str(mp, true);
}

// ================================================================================
MalTypePtr apply(MalTypePtr mp) // FIXME -- reconsider based on mp->lp change in eval
{
    DBG << mp->str(true) << "\n";
    // m can only be a MalListType here
    // exposing iterators was too much leaking abstration.
    return mp->apply();
}

// ================================================================================
std::string rep(std::string s, MalEnvPtr env)
{
    return PRINT(EVAL(READ(s), env));
}

// ================================================================================
MalTypePtr mal_eval(MalTypeIter begin, MalTypeIter end)
{
    return EVAL(*begin, repl_env);
}

// ================================================================================
void setup_repl_env(std::vector<std::string> args)
{
    // setup repl_env core builtins
    for (auto kv : MalCore::ns) {
        repl_env->set(kv.first, kv.second);
    }
    // add eval
    repl_env->set("eval", mal_eval);
    // add commandline arguments
    MalTypePtr argslist = std::make_shared<MalList>('(');
    for (size_t i = 1; i < args.size(); i++) {
        MalTypePtr sp = std::make_shared<MalString>(args[i]);
        std::static_pointer_cast<MalList>(argslist)->add(sp);
    }
    repl_env->set("*ARGV*", argslist);
    repl_env->set("*host-language*", std::make_shared<MalString>("C++"));
    repl_env->set("*version*", std::make_shared<MalString>(RAL_VERSION));
    repl_env->set("*build-type*", std::make_shared<MalString>(RAL_BUILD_TYPE));
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
        while (true) {
            std::string input;
            // FIXME: how to read a multi-line form?
            if (linenoise::Readline("user> ", input)) {
                break;
            }
            try {
                linenoise::AddHistory(input.c_str());
                std::cout << rep(input, repl_env) << std::endl;
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
