// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
//
// types.h - All of the types handled by ral.
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
#pragma once

#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

// ================================================================================
enum class RalKind {
    NONE,
    INTEGER,
    DOUBLE,
    CONSTANT,
    SYMBOL,
    STRING,
    KEYWORD,
    LIST,
    MAP,
    FUNCTION,
    LAMBDA,
    ATOM
};
class RalType;
class RalEnv;
typedef std::shared_ptr<RalEnv> RalEnvPtr;
typedef std::shared_ptr<RalType> RalTypePtr;
typedef std::vector<RalTypePtr>::iterator RalTypeIter;
class RalType : public std::enable_shared_from_this<RalType> {
  public:
    virtual ~RalType(){}; // remember to create a virtual destructor if you have
                          // virtual methods
    virtual RalKind kind() = 0;                 // pure virtual
    virtual std::string str(bool readable) = 0; // pure virtual
    virtual RalTypePtr eval(RalEnvPtr env) = 0; // pure virtual
    virtual bool equal(RalTypePtr that) = 0;    // pure virtual
    // only some types implement the below functions -- they are NOT pure
    // virtual
    virtual RalTypePtr apply(RalTypeIter begin, RalTypeIter end);
    virtual std::string asMapKey();
    virtual int64_t asInt();
    virtual double asDouble();
    virtual bool isNilOrFalse();
    virtual RalTypePtr getMeta();
    virtual void setMeta(RalTypePtr meta);
    // methods below only are used in MalList
    virtual bool isList();
    virtual bool isVector();
    virtual bool isEmptyList();
    virtual RalTypePtr apply();
    virtual void setEnv(RalEnvPtr env);
    virtual bool is_macro_call(RalEnvPtr env);
};

// ================================================================================
class RalInteger : public RalType {
    const std::string repr_;
    const int64_t value_;

  public:
    RalInteger(const std::string &s);
    RalInteger(int64_t i);
    RalInteger(RalInteger *that);
    ~RalInteger() override;
    RalKind kind() override { return RalKind::INTEGER; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    int64_t asInt() override;
    double asDouble() override; // I'm not 100% sure.  90% sure this is right.
};

// ================================================================================
class RalDouble : public RalType {
    const std::string repr_;
    const double value_;

  public:
    RalDouble(const std::string &s);
    RalDouble(double d);
    RalDouble(RalDouble *that);
    ~RalDouble() override;
    RalKind kind() override { return RalKind::DOUBLE; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    double asDouble() override;
};

// ================================================================================
class RalConstant : public RalType {
    const std::string repr_;

  public:
    RalConstant(const std::string &s);
    RalConstant(RalConstant *that);
    ~RalConstant() override;
    RalKind kind() override { return RalKind::CONSTANT; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    int64_t asInt() override;
    double asDouble() override;
    bool isNilOrFalse() override;
};

// ================================================================================
class RalSymbol : public RalType {
    const std::string repr_;

  public:
    RalSymbol(const std::string &s);
    ~RalSymbol() override;
    RalKind kind() override { return RalKind::SYMBOL; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
};

// ================================================================================
std::string transformToPrintable(std::string s);

class RalString : public RalType {
    const std::string repr_;

  public:
    RalString(const std::string &s);
    RalString(RalString *that);
    ~RalString() override;
    RalKind kind() override { return RalKind::STRING; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    std::string asMapKey() override;
};

// ================================================================================
class RalKeyword : public RalType {
    const std::string repr_;

  public:
    RalKeyword(const std::string &s);
    RalKeyword(RalKeyword *that);
    ~RalKeyword() override;
    RalKind kind() override { return RalKind::KEYWORD; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    std::string asMapKey() override;
};

// ================================================================================
class RalList : public RalType {
  protected:
    std::vector<RalTypePtr> values_;
    char listStartChar_; // ( for list, [ for vector
    RalTypePtr meta_;

    std::string listStartStr();
    std::string listEndStr();

  public:
    RalList(char listStartChar);
    RalList(std::shared_ptr<RalList> that);
    ~RalList() override;
    RalKind kind() override { return RalKind::LIST; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    RalTypePtr apply() override;
    void add(RalTypePtr mp);
    RalTypePtr count();
    bool isList() override;
    bool isVector() override;
    bool isEmptyList() override;
    void setEnv(RalEnvPtr env) override;
    virtual bool is_macro_call(RalEnvPtr env) override;
    RalTypePtr get(size_t i);
    size_t size();
    RalTypePtr getMeta() override;
    void setMeta(RalTypePtr meta) override;
};

// ================================================================================
class RalMap : public RalType {
  protected:
    std::map<std::string, RalTypePtr> values_;
    RalTypePtr meta_;

  public:
    RalMap();
    RalMap(std::shared_ptr<RalMap> that);
    ~RalMap() override;
    RalKind kind() override { return RalKind::MAP; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    void add(std::string key, RalTypePtr val);
    RalTypePtr get(RalTypePtr k);
    void remove(RalTypePtr k);
    bool hasKey(RalTypePtr k);
    RalTypePtr getKeys();
    RalTypePtr getVals();
    RalTypePtr getMeta() override;
    void setMeta(RalTypePtr meta) override;
};

// ================================================================================
typedef std::function<RalTypePtr(RalTypeIter, RalTypeIter)>
    RalFunctionSignature;
class RalFunction : public RalType {
    RalFunctionSignature fn_;
    std::string name_;
    RalTypePtr meta_;

  public:
    RalFunction();
    RalFunction(std::shared_ptr<RalFunction> that);
    RalFunction(const std::string &name, RalFunctionSignature fn);
    ~RalFunction() override;
    RalKind kind() override { return RalKind::FUNCTION; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    RalTypePtr apply(RalTypeIter begin, RalTypeIter end) override;
    RalTypePtr getMeta() override;
    void setMeta(RalTypePtr meta) override;
};

// ================================================================================
class RalLambda : public RalType {
    std::vector<RalTypePtr> binds_;
    RalTypePtr form_;
    RalEnvPtr env_;
    bool is_macro_;
    RalTypePtr meta_;

  public:
    RalLambda(std::vector<RalTypePtr> binds, RalTypePtr &form, RalEnvPtr env);
    RalLambda(RalLambda *that);
    RalLambda(std::shared_ptr<RalLambda> that);
    ~RalLambda() override;
    RalKind kind() override { return RalKind::LAMBDA; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    RalTypePtr apply(RalTypeIter begin, RalTypeIter end) override;
    RalEnvPtr makeEnv(RalTypeIter begin, RalTypeIter end);
    RalTypePtr form() { return form_; }
    void set_is_macro() { is_macro_ = true; }
    bool get_is_macro() { return is_macro_; }
    RalTypePtr getMeta() override;
    void setMeta(RalTypePtr meta) override;
};

// ================================================================================
class RalAtom : public RalType {
    RalTypePtr value_;

  public:
    RalAtom(RalTypePtr that);
    ~RalAtom() override;
    RalKind kind() override { return RalKind::ATOM; }
    std::string str(bool readable) override;
    RalTypePtr eval(RalEnvPtr env) override;
    bool equal(RalTypePtr that) override;
    RalTypePtr value();
    RalTypePtr set(RalTypePtr that);
};

// ================================================================================
// FIXME -- should clean up  these errors to accept params & be fewer in number.
// Errors/Exceptions
class RalMissingParen : public std::exception {
    virtual const char *what() const throw()
    {
        return "reached the end of input without finding a ')'";
    }
};

class RalMissingQuote : public std::exception {
    virtual const char *what() const throw()
    {
        return "reached the end of input for a string without finding a '\"'";
    }
};

class RalUnbalancedBackslash : public std::exception {
    virtual const char *what() const throw()
    {
        return "string had unbalanced backslash character";
    }
};

class RalMissingMapValue : public std::exception {
    virtual const char *what() const throw()
    {
        return "reached the end of input for a map without finding a paired "
               "value for a key.";
    }
};

class RalBadKeyType : public std::exception {
    virtual const char *what() const throw()
    {
        return "map key is not a string.";
    }
};

class RalNoIntegerRepresentation : public std::exception {
    virtual const char *what() const throw()
    {
        return "no conversion to integer for this type.";
    }
};

class RalNoDoubleRepresentation : public std::exception {
    virtual const char *what() const throw()
    {
        return "no conversion to double for this type.";
    }
};

class RalNotApplicable : public std::exception {
    virtual const char *what() const throw()
    {
        return "cannot apply() this type.";
    }
};

class RalNotInEnvironment : public std::exception {
    std::string msg_;
    virtual const char *what() const throw() { return msg_.c_str(); }

  public:
    RalNotInEnvironment(std::string name)
    {
        msg_ = "'" + name + "' not found"; //  in the environment
    }
};

class RalBadSetEnv : public std::exception {
    virtual const char *what() const throw()
    {
        return "cannot setEnv() this type.";
    }
};

class RalBadSetEnvList : public std::exception {
    virtual const char *what() const throw()
    {
        return "let* environment list must have an even number of values.";
    }
};

class RalBadFnParam1 : public std::exception {
    virtual const char *what() const throw()
    {
        return "fn* 1st argument must be a list.";
    }
};

class RalIndexOutOfRange : public std::exception {
    virtual const char *what() const throw()
    {
        return "nth index is out of range.";
    }
};

class RalException : public std::exception {
    std::string msg_;
    virtual const char *what() const throw() { return msg_.c_str(); }

  public:
    RalException(std::string msg) { msg_ = msg; }
};