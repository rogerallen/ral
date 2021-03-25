// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
//
// ral_stdlib.h - std library definitions
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

const char *RAL_STDLIB_FORMS[] = {
    "(def! not (fn* (a) (if a false true)))", // FORM 1

    "(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", // FORM 2
    
    "(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) "
    "(if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to "
    "cond\")) (cons 'cond (rest (rest xs)))))))", // FORM 3
    
    "(def! radians (fn* (deg) (* TAU (/ deg 360.))))", // FORM 4

    "(def! degrees (fn* (rad) (* 360. (/ rad TAU))))", // FORM 5
    
    "(defmacro! defn! (fn* (name args body) `(def! ~name (fn* ~args ~body))))", // FORM 6

    // ================================================================================
    // Ral "Standard" Library 
    // Grabbed the best code from https://github.com/kanaka/mal/tree/master/impls/lib
    // ================================================================================

    // --------------------------------------------------------------------------------
    // alias-hacks.mal
    "(def! partial (fn* [pfn & args]"
    "  (fn* [& args-inner]"
    "    (apply pfn (concat args args-inner)))))", // FORM 7

    // --------------------------------------------------------------------------------
    // load-file-once.mal
    //
    // Like load-file, but will never load the same path twice.
    //
    // since this is in our std library, we don't need the
    // special handling for load-file-once of this containing file.
    "(def! load-file-once"
    "  (let* [seen (atom {})]"
    "    (fn* [filename]"
    "      (if (not (contains? @seen filename))"
    "        (do"
    "          (swap! seen assoc filename nil)"
    "         (load-file filename))))))", // FORM 8

    // --------------------------------------------------------------------------------
    // memoize.mal

    // Memoize any function.

    // Implement `memoize` using an atom (`mem`) which holds the memoized results
    // (hash-map from the arguments to the result). When the function is called,
    // the hash-map is checked to see if the result for the given argument was already
    // calculated and stored. If this is the case, it is returned immediately;
    // otherwise, it is calculated and stored in `mem`.

    // For recursive functions, take care to store the wrapper under the
    // same name than the original computation with an assignment like
    // `(def! f (memoize f))`, so that intermediate results are memorized.

    // Adapted from http://clojure.org/atoms

"(def! memoize"
"  (fn* [f]"
"    (let* [mem (atom {})]"
"      (fn* [& args]"
"        (let* [key (str args)]"
"          (if (contains? @mem key)"
"            (get @mem key)"
"            (let* [ret (apply f args)]"
"              (do"
"                (swap! mem assoc key ret)"
"                ret))))))))", // FORM 9

    // --------------------------------------------------------------------------------
    // trivial.mal

    // Trivial but convenient functions.

    // Integer predecessor (number -> number)
    "(def! inc (fn* [a] (+ a 1)))", // FORM 10

    // Integer predecessor (number -> number)
    "(def! dec (fn* (a) (- a 1)))", // FORM 11

    // Integer nullity test (number -> boolean)
    "(def! zero? (fn* (n) (= 0 n)))", // FORM 12

    // Returns the unchanged argument.
    "(def! identity (fn* (x) x))", // FORM 13

    // Generate a hopefully unique symbol. See section "Plugging the Leaks"
    // of http://www.gigamonkeys.com/book/macros-defining-your-own.html
    "(def! gensym"
    "  (let* [counter (atom 0)]"
    "    (fn* []"
    "      (symbol (str \"G__\" (swap! counter inc))))))", // FORM 14

    // --------------------------------------------------------------------------------
    // performance.mal

    // Measure performances.

    // Evaluate an expression, but report the time spent
    "(defmacro! time"
    "  (fn* (exp)"
    "    (let* [start (gensym)"
    "           ret   (gensym)]"
    "      `(let* (~start (time-ms)"
    "              ~ret   ~exp)"
    "        (do"
    "          (println \"Elapsed time:\" (- (time-ms) ~start) \"msecs\")"
    "          ~ret)))))", // FORM 15

    // Count evaluations of a function during a given time frame.
    "(def! run-fn-for"
    ""
    "  (let* ["
    "    run-fn-for* (fn* [fn max-ms acc-ms last-iters]"
    "      (let* [start (time-ms)"
    "             _ (fn)"
    "             elapsed (- (time-ms) start)"
    "             iters (inc last-iters)"
    "             new-acc-ms (+ acc-ms elapsed)]"
    "        ;; (do (prn \"new-acc-ms:\" new-acc-ms \"iters:\" iters))\n"
    "        (if (>= new-acc-ms max-ms)"
    "          last-iters"
    "          (run-fn-for* fn max-ms new-acc-ms iters))))"
    "    ]"
    ""
    "    (fn* [fn max-secs]"
    "       ;; fn       : function without parameters\n"
    "       ;; max-secs : number (seconds)\n"
    "       ;; return   : number (iterations)\n"
    "       (do"
    "          ;; Warm it up first\n"
    "          (run-fn-for* fn 1000 0 0)"
    "          ;; Now do the test\n"
    "          (run-fn-for* fn (* 1000 max-secs) 0 0)))))", // FORM 16

    // --------------------------------------------------------------------------------
    // pprint.mal

    // Pretty printer a MAL object.

    "(def! pprint"
    ""
    "  (let* ["
    ""
    "    spaces- (fn* [indent]"
    "      (if (> indent 0)"
    "        (str " " (spaces- (- indent 1)))"
    "        \"\"))"
    ""
    "    pp-seq- (fn* [obj indent]"
    "      (let* [xindent (+ 1 indent)]"
    "        (apply str (pp- (first obj) 0)"
    "                   (map (fn* [x] (str \"\n\" (spaces- xindent)"
    "                                      (pp- x xindent)))"
    "                        (rest obj)))))"
    ""
    "    pp-map- (fn* [obj indent]"
    "      (let* [ks (keys obj)"
    "             kindent (+ 1 indent)"
    "             kwidth (count (seq (str (first ks))))"
    "             vindent (+ 1 (+ kwidth kindent))]"
    "        (apply str (pp- (first ks) 0)"
    "                   \" \""
    "                   (pp- (get obj (first ks)) 0)"
    "                   (map (fn* [k] (str \"\n\" (spaces- kindent)"
    "                                      (pp- k kindent)"
    "                                      \" \""
    "                                      (pp- (get obj k) vindent)))"
    "                        (rest (keys obj))))))"
    ""
    "    pp- (fn* [obj indent]"
    "      (cond"
    "        (list? obj)   (str \"(\" (pp-seq- obj indent) \")\")"
    "        (vector? obj) (str \"[\" (pp-seq- obj indent) \"]\")"
    "        (map? obj)    (str \"{\" (pp-map- obj indent) \"}\")"
    "        :else         (pr-str obj)))"
    ""
    "    ]"
    ""
    "    (fn* [obj]"
    "         (println (pp- obj 0)))))", // FORM 17

    // --------------------------------------------------------------------------------
    // reducers.mal

    // Left and right folds.

    // Left fold (f (.. (f (f init x1) x2) ..) xn)
    "(def! reduce"
    "  (fn* (f init xs)"
    "    ;; f      : Accumulator Element -> Accumulator\n"
    "    ;; init   : Accumulator\n"
    "    ;; xs     : sequence of Elements x1 x2 .. xn\n"
    "    ;; return : Accumulator\n"
    "    (if (empty? xs)"
    "      init"
    "      (reduce f (f init (first xs)) (rest xs)))))", // FORM 18

    // Right fold (f x1 (f x2 (.. (f xn init)) ..))
    // The natural implementation for `foldr` is not tail-recursive, and
    // the one based on `reduce` constructs many intermediate functions, so we
    // rely on efficient `nth` and `count`.
    "(def! foldr"
    ""
    "  (let* ["
    "    rec (fn* [f xs acc index]"
    "      (if (< index 0)"
    "        acc"
    "        (rec f xs (f (nth xs index) acc) (- index 1))))"
    "    ]"
    ""
    "    (fn* [f init xs]"
    "        ;; f      : Element Accumulator -> Accumulator\n"
    "        ;; init   : Accumulator\n"
    "        ;; xs     : sequence of Elements x1 x2 .. xn\n"
    "        ;; return : Accumulator\n"
    "       (rec f xs init (- (count xs) 1)))))", // FORM 19

    // --------------------------------------------------------------------------------
    // test_cascade.mal

    // Iteration on evaluations interpreted as boolean values.

    // `(cond test1 result1 test2 result2 .. testn resultn)`
    // is rewritten (in the step files) as
    // `(if test1 result1 (if test2 result2 (.. (if testn resultn nil))))`
    // It is common that `testn` is `"else"`, `:else`, `true` or similar.

    // `(or x1 x2 .. xn x)`
    // is almost rewritten as
    // `(if x1 x1 (if x2 x2 (.. (if xn xn x))))`
    // except that each argument is evaluated at most once.
    // Without arguments, returns `nil`.
    "(defmacro! or (fn* [& xs]"
    "  (if (< (count xs) 2)"
    "    (first xs)"
    "    (let* [r (gensym)]"
    "      `(let* (~r ~(first xs)) (if ~r ~r (or ~@(rest xs))))))))", // FORM 20

    // Conjonction of predicate values (pred x1) and .. and (pred xn)
    // Evaluate `pred x` for each `x` in turn. Return `false` if a result
    // is `nil` or `false`, without evaluating the predicate for the
    // remaining elements.  If all test pass, return `true`.
    "(def! every?"
    "  (fn* (pred xs)"
    "    ;; pred   : Element -> interpreted as a logical value\n"
    "    ;; xs     : sequence of Elements x1 x2 .. xn\n"
    "    ;; return : boolean\n"
    "    (cond (empty? xs)       true"
    "          (pred (first xs)) (every? pred (rest xs))"
    "          true              false)))", // FORM 21

    // Disjonction of predicate values (pred x1) or .. (pred xn)
    // Evaluate `(pred x)` for each `x` in turn. Return the first result
    // that is neither `nil` nor `false`, without evaluating the predicate
    // for the remaining elements.  If all tests fail, return nil.
    "(def! some"
    "  (fn* (pred xs)"
    "    ;; pred   : Element -> interpreted as a logical value\n"
    "    ;; xs     : sequence of Elements x1 x2 .. xn\n"
    "    ;; return : boolean\n"
    "    (if (empty? xs)"
    "      nil"
    "      (or (pred (first xs))"
    "          (some pred (rest xs))))))", // FORM 22

    // Search for first evaluation returning `nil` or `false`.
    // Rewrite `x1 x2 .. xn x` as
    //   (let* [r1 x1]
    //     (if r1 test1
    //       (let* [r2 x2]
    //         ..
    //         (if rn
    //           x
    //           rn) ..)
    //       r1))
    // Without arguments, returns `true`.
    "(defmacro! and"
    "  (fn* (& xs)"
    "    ;; Arguments and the result are interpreted as boolean values.\n"
    "    (cond (empty? xs)      true"
    "          (= 1 (count xs)) (first xs)"
    "          true             (let* (condvar (gensym))"
    "                             `(let* (~condvar ~(first xs))"
    "                               (if ~condvar (and ~@(rest xs)) ~condvar))))))", // FORM 23

    // --------------------------------------------------------------------------------
    // threading.mal

    // Composition of partially applied functions.

    // Rewrite x (a a1 a2) .. (b b1 b2) as
    //   (b (.. (a x a1 a2) ..) b1 b2)
    // If anything else than a list is found were `(a a1 a2)` is expected,
    // replace it with a list with one element, so that `-> x a` is
    // equivalent to `-> x (list a)`.
    "(defmacro! ->"
    "  (fn* (x & xs)"
    "    (reduce _iter-> x xs)))", // FORM 24

    "(def! _iter->"
    "  (fn* [acc form]"
    "    (if (list? form)"
    "      `(~(first form) ~acc ~@(rest form))"
    "      (list form acc))))", // FORM 25

    // Like `->`, but the arguments describe functions that are partially
    // applied with *left* arguments.  The previous result is inserted at
    // the *end* of the new argument list.
    // Rewrite x ((a a1 a2) .. (b b1 b2)) as
    //   (b b1 b2 (.. (a a1 a2 x) ..)).
    "(defmacro! ->>"
    "  (fn* (x & xs)"
    "     (reduce _iter->> x xs)))", // FORM 26

    "(def! _iter->>"
    "  (fn* [acc form]"
    "    (if (list? form)"
    "      `(~(first form) ~@(rest form) ~acc)"
    "      (list form acc))))", // FORM 27

};

#define NUM_RAL_STDLIB_FORMS 27