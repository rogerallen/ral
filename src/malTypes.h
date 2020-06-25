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

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

// ================================================================================
enum class MalKind {
    NONE,
    INTEGER,
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
class MalType;
class MalEnv;
typedef std::shared_ptr<MalEnv> MalEnvPtr;
typedef std::shared_ptr<MalType> MalTypePtr;
typedef std::vector<MalTypePtr>::iterator MalTypeIter;
class MalType {
  public:
    virtual ~MalType(){};                       // remember to create a virtual destructor if you have virtual methods
    virtual MalKind kind() = 0;                 // pure virtual
    virtual std::string str(bool readable) = 0; // pure virtual
    virtual MalTypePtr eval(MalEnvPtr env) = 0; // pure virtual
    virtual bool equal(MalTypePtr that) = 0;   // pure virtual
    // only some types implement the below functions
    virtual MalTypePtr apply(MalTypeIter begin, MalTypeIter end); // NOT pure virtual
    virtual std::string asMapKey();                               // NOT pure virtual
    virtual int asInt();                                          // NOT pure virtual
    virtual bool isNilOrFalse();                                  // NOT pure virtual
    virtual MalTypePtr getMeta();                                 // NOT pure virtual
    virtual void setMeta(MalTypePtr meta);                        // NOT pure virtual
    // only list methods below
    virtual bool isList();              // NOT pure virtual
    virtual bool isVector();            // NOT pure virtual
    virtual bool isEmptyList();         // NOT pure virtual
    virtual MalTypePtr apply();         // NOT pure virtual
    virtual void setEnv(MalEnvPtr env); // NOT pure virtual
    virtual bool is_macro_call(MalEnvPtr env); // NOT pure virtual
};

// ================================================================================
class MalInteger : public MalType {
    std::string repr_;
    int64_t value_;

  public:
    MalInteger(std::string s);
    MalInteger(int i);
    MalInteger(MalInteger *that);
    ~MalInteger() override;
    MalKind kind() override { return MalKind::INTEGER; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    int asInt() override;
};

// ================================================================================
class MalConstant : public MalType {
    std::string repr_;

  public:
    MalConstant(std::string s);
    MalConstant(MalConstant *that);
    ~MalConstant() override;
    MalKind kind() override { return MalKind::CONSTANT; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    int asInt() override;
    bool isNilOrFalse() override;
};

// ================================================================================
class MalSymbol : public MalType {
    std::string repr_;

  public:
    MalSymbol(std::string s);
    ~MalSymbol() override;
    MalKind kind() override { return MalKind::SYMBOL; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
};

// ================================================================================
std::string transformToPrintable(std::string s);

class MalString : public MalType {
    std::string repr_;

  public:
    MalString(std::string s);
    MalString(MalString *that);
    ~MalString() override;
    MalKind kind() override { return MalKind::STRING; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    std::string asMapKey() override;
};

// ================================================================================
class MalKeyword : public MalType {
    std::string repr_;

  public:
    MalKeyword(std::string s);
    MalKeyword(MalKeyword *that);
    ~MalKeyword() override;
    MalKind kind() override { return MalKind::KEYWORD; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    std::string asMapKey() override;
};

// ================================================================================
class MalList : public MalType {
  protected:
    std::vector<MalTypePtr> values_;
    char listStartChar_; // ( for list, [ for vector
    MalTypePtr meta_;
    
    std::string listStartStr();
    std::string listEndStr();

  public:
    MalList(char listStartChar);
    MalList(std::shared_ptr<MalList> that);
    ~MalList() override;
    MalKind kind() override { return MalKind::LIST; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    MalTypePtr apply() override;
    void add(MalTypePtr mp);
    MalTypePtr count();
    bool isList() override;
    bool isVector() override;
    bool isEmptyList() override;
    void setEnv(MalEnvPtr env) override;
    virtual bool is_macro_call(MalEnvPtr env) override;
    MalTypePtr doList(MalEnvPtr env);
    MalTypePtr get(unsigned int i);
    size_t size();
    MalTypePtr getMeta() override;
    void setMeta(MalTypePtr meta) override;
};

// ================================================================================
class MalMap : public MalType {
  protected:
    std::map<std::string, MalTypePtr> values_;
    MalTypePtr meta_;

  public:
    MalMap();
    MalMap(std::shared_ptr<MalMap> that);
    ~MalMap() override;
    MalKind kind() override { return MalKind::MAP; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    void add(std::string key, MalTypePtr val);
    MalTypePtr get(MalTypePtr k);
    void remove(MalTypePtr k);
    bool hasKey(MalTypePtr k);
    MalTypePtr getKeys();
    MalTypePtr getVals();
    MalTypePtr getMeta() override;
    void setMeta(MalTypePtr meta) override;
};

// ================================================================================
typedef std::function<MalTypePtr(MalTypeIter, MalTypeIter)> MalFunctionSignature;
class MalFunction : public MalType {
    MalFunctionSignature fn_;
    std::string name_;
    MalTypePtr meta_;

  public:
    MalFunction();
    MalFunction(std::string name, MalFunctionSignature fn);
    ~MalFunction() override;
    MalKind kind() override { return MalKind::FUNCTION; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    MalTypePtr apply(MalTypeIter begin, MalTypeIter end) override;
    MalTypePtr getMeta() override;
    void setMeta(MalTypePtr meta) override;
};

// ================================================================================
class MalLambda : public MalType {
    std::vector<MalTypePtr> binds_;
    MalTypePtr form_;
    MalEnvPtr env_;
    bool is_macro_;
    MalTypePtr meta_;

  public:
    MalLambda(std::vector<MalTypePtr> binds, MalTypePtr &form, MalEnvPtr env);
    MalLambda(std::shared_ptr<MalLambda> that);
    ~MalLambda() override;
    MalKind kind() override { return MalKind::LAMBDA; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    MalTypePtr apply(MalTypeIter begin, MalTypeIter end) override;
    MalEnvPtr makeEnv(MalTypeIter begin, MalTypeIter end);
    MalTypePtr form() { return form_; }
    void set_is_macro() { is_macro_ = true; }
    bool get_is_macro() { return is_macro_; }
    MalTypePtr getMeta() override;
    void setMeta(MalTypePtr meta) override;
};

// ================================================================================
class MalAtom : public MalType {
    MalTypePtr value_;

  public:
    MalAtom(MalTypePtr that);
    ~MalAtom() override;
    MalKind kind() override { return MalKind::ATOM; }
    std::string str(bool readable) override;
    MalTypePtr eval(MalEnvPtr env) override;
    bool equal(MalTypePtr that) override;
    MalTypePtr value();
    MalTypePtr set(MalTypePtr that);
};

// ================================================================================
// FIXME -- should clean up  these errors to accept params & be fewer in number.
// Errors/Exceptions
class MalMissingParen : public std::exception {
    virtual const char *what() const throw()
    {
        return "reached the end of input without finding a ')'";
    }
};

class MalMissingQuote : public std::exception {
    virtual const char *what() const throw()
    {
        return "reached the end of input for a string without finding a '\"'";
    }
};

class MalUnbalancedBackslash : public std::exception {
    virtual const char *what() const throw()
    {
        return "string had unbalanced backslash character";
    }
};

class MalMissingMapValue : public std::exception {
    virtual const char *what() const throw()
    {
        return "reached the end of input for a map without finding a paired value for a key.";
    }
};

class MalBadKeyType : public std::exception {
    virtual const char *what() const throw()
    {
        return "map key is not a string.";
    }
};

class MalNoIntegerRepresentation : public std::exception {
    virtual const char *what() const throw()
    {
        return "no conversion to integer for this type.";
    }
};

class MalNotApplicable : public std::exception {
    virtual const char *what() const throw()
    {
        return "cannot apply() this type.";
    }
};

class MalNotInEnvironment : public std::exception {
    std::string msg_;
    virtual const char *what() const throw()
    {
        return msg_.c_str();
    }

  public:
    MalNotInEnvironment(std::string name)
    {
        msg_ = "'" + name + "' not found"; //  in the environment
    }
};

class MalBadSetEnv : public std::exception {
    virtual const char *what() const throw()
    {
        return "cannot setEnv() this type.";
    }
};

class MalBadSetEnvList : public std::exception {
    virtual const char *what() const throw()
    {
        return "let* environment list must have an even number of values.";
    }
};

class MalBadFnParam1 : public std::exception {
    virtual const char *what() const throw()
    {
        return "fn* 1st argument must be a list.";
    }
};

class MalIndexOutOfRange : public std::exception {
    virtual const char *what() const throw()
    {
        return "nth index is out of range.";
    }
};


class MalException : public std::exception {
    std::string msg_;
    virtual const char *what() const throw()
    {
        return msg_.c_str();
    }

  public:
    MalException(std::string msg)
    {
        msg_ = msg;
    }
};