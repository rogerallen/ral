# Ral


## Math

### Unary
* `(- a)`: [core] negate
* `(sqrt a)`: [core] square root
* `(sin a)`: [core] sine
* `(cos a)`: [core] cosine
* `(abs a)`: [core] absolute value

### Binary
* `(+ a b)`: [core] add
* `(- a b)`: [core] subtract
* `(* a b)`: [core] multiply
* `(/ a b)`: [core] divide

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

### Lists
* `(list a ...)`: [core] list
* `(count a)`: [core] count
* `(cons a b)`: [core] cons
* `(concat a b)`: [core] concat
* `(nth a b)`: [core] nth
* `(first a b)`: [core] first
* `(rest a b)`: [core] rest
* `(map a b)`: [core] map
* `(seq a b)`: [core] seq
* `(conj a b)`: [core] conj

### Strings & Printing
* `(pr-str a)`: [core] pr_str
* `(str a)`: [core] str
* `(prn a)`: [core] prn
* `(println a)`: [core] println

### System Input
* `(read-string)`: [core] read_string
* `(slurp a)`: [core] slurp
* `(readline a b)`: [core] readline
* `(time-ms a b)`: [core] time_ms

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
