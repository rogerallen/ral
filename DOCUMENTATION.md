# Roger Allen's Lisp (Ral)

I Made a Lisp based on https://github.com/kanaka/mal

There are many others like it, but this one is mine.

## Command line options
* `-v`: once for info, twice for debug, thrice for even moar debug.  not included in `*ARGV*`
* other arguments prefixed by '-' are added to `*ARGV*`
* if any arguments remain, the first argument is used as a filename passed to `load-file`.  

## Environment
* `*ARGV*`: argument list
* `*host-language*`: Ral returns "C++"
* `*version*`: current version
* `*build-type*`: Debug or Release

The last 50 commands are kept in the  file `history.txt` stored in the current working directory.

Functions ending in '!' modify state.  `def!`, `defmacro!`, `defn!`, `reset!`, `swap!`

## Control
* `(def! symbol value)`: [special] update env with symbol & EVAL(value)
* `(defmacro! symbol value)`: [special]
* `(macroexapand macro)`: [special]
* `(let* (sym1 val1 ...) form)`: [special] create new letEnv and EVAL(form,letEnv)
* `(do ...)`: [special] evaluate the forms in order
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
* `(cond ...)`: [ral] conditional execution
* `(defn! name args body)`: [ral] define function

## Printing
* `(pr-str a)`: [core] pr_str
* `(prn a)`: [core] prn
* `(println a)`: [core] println

## System Input
* `(read-string)`: [core] read_string
* `(slurp a)`: [core] read in file a
* `(readline a b)`: [core] readline
* `(time-ms a b)`: [core] time_ms
* `(load-file a)`: [ral] read in and evaluate file named a

## Math

### Unary Operations
* `(- a)`: [core] negate
* `(sqrt a)`: [core] square root of a
* `(sin a)`: [core] sine of a in radians
* `(cos a)`: [core] cosine of a in radians
* `(abs a)`: [core] absolute value
* `(not a)`: [ral] if a evaluates to true return false, else return true
* `(radians deg)`: [ral] converts deg to radians
* `(degrees rad)`: [ral] converts rad to degrees

### Operations
* `(+ a ...)`: [core] add items
* `(- a ...)`: [core] subtract items
* `(* a ...)`: [core] multiply items
* `(/ a ...)`: [core] divide items

### Unary Conditionals
* `(list? a)`: [core] is a list
* `(empty? a)`: [core] is an empty list
* `(atom? a)`: [core] is an atom
* `(nil? a)`: [core] nil_q
* `(true? a)`: [core] true_q
* `(false? a)`: [core] false_q
* `(symbol? a)`: [core] symbol_q
* `(keyword? a)`: [core] keyword_q
* `(vector? a)`: [core] vector_q
* `(sequential? a)`: [core] sequential_q
* `(map? a)`: [core] map_q
* `(fn? a)`: [core] fn_q
* `(string? a)`: [core] string_q
* `(number? a)`: [core] number_q
* `(macro? a)`: [core] macro_q
* `(contains? a)`: [core] contains_q

### Binary Conditionals
* `(= a b)`: [core] equal
* `(< a b)`: [core] lt
* `(<= a b)`: [core] le
* `(> a b)`: [core] gt
* `(>= a b)`: [core] ge

## Data Structures

### Operations on Sequences
* `(count a)`: [core] count
* `(concat a b)`: [core] concat
* `(nth a b)`: [core] nth
* `(first a b)`: [core] first
* `(rest a b)`: [core] rest
* `(map a b)`: [core] map
* `(seq a b)`: [core] seq
* `(conj a b)`: [core] conj

### Lists
* `(list a ...)`: [core] list
* `(cons a b)`: [core] cons

### Strings 
* `(str a)`: [core] str

### Atoms
* `(atom a b)`: [core] atom
* `(deref a b)`: [core] deref
* `(reset! a b)`: [core] reset
* `(swap! a b)`: [core] swap

### Maps
* `(hash-map a b)`: [core] hash_map
* `(assoc a b)`: [core] assoc
* `(dissoc a b)`: [core] dissoc
* `(get a b)`: [core] get
* `(keys a b)`: [core] keys
* `(vals a b)`: [core] vals

### Symbols
* `(symbol a b)`: [core] symbol

### Keywords
* `(keyword a b)`: [core] keyword

### Vectors
* `(vector a b)`: [core] vector

## Exceptions
* `(throw a b)`: [core] throw
* `(apply a b)`: [core] apply

## Meta
* `(meta a b)`: [core] meta
* `(with-meta a b)`: [core] with_meta
