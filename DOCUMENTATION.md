# Roger Allen's Lisp (Ral)

I Made a Lisp based on https://github.com/kanaka/mal

There are many others like it, but this one is mine.

## Command line options
* `-v`: add once for info, twice for debug, thrice for even moar debug.  not included in `*ARGV*`
* other arguments prefixed by '-' are added to `*ARGV*`
* if any arguments remain, the first argument is used as a filename passed to `load-file`.  

## Environment
* `*ARGV*`: argument list
* `*host-language*`: Ral returns "C++"
* `*version*`: current version
* `*build-type*`: Debug or Release
* `(eval form)`: [core] evaluate form and update the environment

The last 50 commands are kept in the  file `history.txt` stored in the current working directory.

Functions ending in '!' modify state.  `def!`, `defmacro!`, `defn!`, `reset!`, `swap!`

## Control
* `(def! symbol value)`: [special] update env with symbol containing value
* `(defmacro! symbol value)`: [special] update env with symbol containing value as a macro
* `(macroexapand macro)`: [special] expand macro
* `(let* (sym1 val1 ...) form)`: [special] create new environment with symbols & values and return evaluated form
* `(do ...)`: [special] evaluate the forms in order, returning the value of the last form
* `(if condition true-form false-form)`: [special] evaluate true-form if the condition evaluates to true, else evaluate the false-form
* `(fn* binding-list form)`: [special] return a lambda that can be called later.
* `(quote a)`: [special] return a without evaluating it
* `(quasiquote ...)`: [special] quasiquote.  
  if ... is not a list, 
     quote the list.  
  else is a list
    if first element is "unquote", 
      return second element of list
    else splice-unquote
      xxx concat
    else 
      xxx cons
* `(try* a (catch * b c))`: [special] a (native language) try/catch block
* `(cond ...)`: [stdlib] conditional execution
* `(defn! name args body)`: [stdlib] define function

## Printing
* `(pr-str ...)`: [core] returns evaluation of args as a string.  
* `(prn ...)`: [core] evaluate args and print to stdout.  returns nil
* `(println ...)`: [core] evaluate args and print to stdout, followed by return char.  returns nil

## System Input
* `(load-file a)`: [stdlib] read in and evaluate file named a
* `(readline a)`: [core] prints a as prompt and returns string from user input
* `(slurp a)`: [core] read in file named a and returns it as a string
* `(time-ms)`: [core] return system time in ms

## Math

### Unary Operations
* `(abs a)`: [core] absolute value
* `(cos a)`: [core] cosine of a in radians
* `(degrees rad)`: [stdlib] converts rad to degrees
* `(not a)`: [stdlib] if a evaluates to true return false, else return true
* `(radians deg)`: [stdlib] converts deg to radians
* `(sin a)`: [core] sine of a in radians
* `(sqrt a)`: [core] square root of a

### Operations
* `(+ ...)`: [core] add items
* `(- ...)`: [core] subtract items [NOTE: not unary negate]
* `(* ...)`: [core] multiply items
* `(/ ...)`: [core] divide items

### Unary Conditionals
* `(atom? a)`: [core] return true if a is an atom
* `(empty? a)`: [core] return true if a is a empty list or empty vector
* `(false? a)`: [core] return true if a is false
* `(fn? a)`: [core] return true if a is a function
* `(keyword? a)`: [core] return true if a is a keyword
* `(list? a)`: [core] return true if a is a list (not a vector)
* `(macro? a)`: [core] return true if a is a macro
* `(map? a)`: [core] return true if a is a hash-map
* `(nil? a)`: [core] return true if a is nil
* `(number? a)`: [core] return true if a is a number
* `(sequential? a)`: [core] return true if a is a list or vector
* `(string? a)`: [core] return true if a is a string
* `(symbol? a)`: [core] return true if a is a symbol
* `(true? a)`: [core] return true if a is true
* `(vector? a)`: [core] return true if a is a vector

### Binary Conditionals
* `(= a b)`: [core] returns true if a and b evaluate to equal values
* `(< a b)`: [core] ditto less than
* `(<= a b)`: [core] ditto less than or equal to
* `(> a b)`: [core] ditto greater than
* `(>= a b)`: [core] ditto greather than or equal to

## Data Structures

* `(read-string a)`: [core] parses a and returns the value as a proper ral data structure type

### Operations on Sequences
* `(concat ...)`: [core] concatenate items, returning a new list.  All items must be sequences.
* `(conj a ...)`: [core] add items to a returning the same sequence type.  If a is a list, items are prepended, if a vector items are appended.
  user> (conj '(:a :b) :c :d)
  (:d :c :a :b)
  user> (conj [:a :b] :c :d)
  [:a :b :c :d]
* `(cons a seq)`: [core] prepend a onto sequence seq.  returns list.
* `(count a)`: [core] return count of items in sequence a
* `(first a)`: [core] return first item in sequence a
* `(map fn seq)`: [core] map function fn onto each item in sequence seq, returning list
* `(nth seq n)`: [core] return item n from sequence seq
* `(rest a)`: [core] return item list after the first in sequence a 
* `(seq a)`: [core] takes a list, vector, string, or nil. If an empty list, empty vector, or empty string ("") is passed in then nil is returned. Otherwise, a list is returned unchanged, a vector is converted into a list, and a string is converted to a list that containing the original string split into single character strings.

### Lists
* `(list ...)`: [core] creates list of items

### Strings 
* `(str ...)`: [core] creates string from items

### Atoms
* `(atom a)`: [core] create and return atom with value of a
* `(deref atm)`: [core] deref atom a, returning value
* `(reset! atm b)`: [core] change the value of atom atm to b
* `(swap! atm fn [args ...])`: [core] The atom's value is modified to the result of applying the function with the atom's value as the first argument and the optionally given function arguments as the rest of the arguments. The new atom's value is returned.

### Hash Maps
* `{:key1 val1 :key2 val2}`: reader macro for hash-map
* `(hash-map a b ...)`: [core] returns hash-map using pairs of key & values
* `(contains? a b)`: [core] return true if hash-map a contains b
* `(assoc a b c)`: [core] return hash-map a with new key b and value c
* `(dissoc a b)`: [core] return hash-map a without key b
* `(get a b)`: [core] get value of key b from hash-map a
* `(keys a)`: [core] returns keys of hash-map a
* `(vals a)`: [core] returns values of hash-map a

### Symbols
* `(symbol a)`: [core] returns symbol named a
  user> (symbol 'abc)
  abc

### Keywords
* `:a`: reader macro for symbol
* `(keyword a)`: [core] returns keyword named a
  user> (keyword 'abc)
  :abc

### Vectors
* `[a b c]`: reader macro fro vector
* `(vector ...)`: [core] returns vector of arguments
  user> (vector 1 2 3)
  [1 2 3]

## Exceptions
* `(throw a b)`: [core] throw exception a

## Apply
* `(apply fn arg1 [arg2 ...])`: [core] takes at least two arguments. The first argument is a function and the last argument is list (or vector). The arguments between the function and the last argument (if there are any) are concatenated with the final argument to create the arguments that are used to call the function. The apply function allows a function to be called with arguments that are contained in a list (or vector). In other words, (apply F A B [C D]) is equivalent to (F A B C D).

## Meta
* `(meta a b)`: [core] meta
* `^a`: reader macro for with-meta
* `(with-meta a b)`: [core] with_meta

## Standard Library
* `(partial fn args ...)`: [stdlib]
* `(load-file-once path)`: [stdlib] Like load-file, but will never load the same path twice.
* `(memoize fn)`: [stdlib] Memoize function fn
* `(inc a)`: [stdlib] integer successor to a
* `(dec a)`: [stdlib] integer predecessor to a
* `(zero? a)`: [stdlib] integer null test
* `(identity a)`: [stdlib] return argument unchanged
* `(gensym a)`: [stdlib] Generate a hopefully unique symbol. 
* `(time a)`: [stdlib] Evaluate an expression, and report the time spent
* `(fun-fn-for fn max-secs)`: [stdlib] Count evaluations of a function during a given time frame.
* `(pprint a)`: [stdlib] pretty print a ral object
* `(reduce f init xs)`: [stdlib] left fold of (f (f (f init x1) x2) x3)
* `(foldr f xs acc index)`: [stdlib] right fold of (f x1 (f x2 (f x3 init)))
* `(and a ...)`: [stdlib] Search for first evaluation returning `nil` or `false`.  Without arguments, returns `true`.
* `(or a ...)`: [stdlib] Search for first evaluation returning `true`.  Without arguments, returns `nil`.
* `(every? pred xs)`: [stdlib] Conjunction of predicate values (pred x1) and .. and (pred xn).  Evaluate `pred x` for each `x` in turn. Return `false` if a result is `nil` or `false`, without evaluating the predicate for the remaining elements.  If all test pass, return `true`.
* `(some pred xs)`: [stdlib] Disjunction of predicate values (pred x1) or .. (pred xn). Evaluate `(pred x)` for each `x` in turn. Return the first result that is neither `nil` nor `false`, without evaluating the predicate for the remaining elements.  If all tests fail, return `nil`.
* `(-> x xs)`: [stdlib] Rewrite `x (a a1 a2) .. (b b1 b2)` as `(b (.. (a x a1 a2) ..) b1 b2)`. If anything else than a list is found were `(a a1 a2)` is expected, replace it with a list with one element, so that `-> x a` is equivalent to `-> x (list a)`.
* `(->> x xs)`: [stdlib] Like `->`, but the arguments describe functions that are partially  applied with *left* arguments.  The previous result is inserted at the *end* of the new argument list. Rewrite `x ((a a1 a2) .. (b b1 b2))` as `(b b1 b2 (.. (a a1 a2 x) ..))`.
