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

#include "malEnv.h"
#include "malTypes.h"
#include <map>
#include <string>

MalTypePtr mal_add(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_sub(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_mul(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_div(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_list(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_list_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_empty_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_count(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_equal(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_lt(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_le(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_gt(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_ge(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_pr_str(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_str(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_prn(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_println(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_read_string(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_slurp(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_atom(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_atom_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_deref(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_reset(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_swap(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_cons(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_concat(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_nth(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_first(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_rest(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_throw(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_apply(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_map(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_nil_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_true_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_false_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_symbol_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_symbol(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_keyword(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_keyword_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_vector(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_vector_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_sequential_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_hash_map(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_map_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_assoc(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_dissoc(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_get(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_contains_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_keys(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_vals(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_readline(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_time_ms(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_meta(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_with_meta(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_fn_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_string_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_number_q(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_seq(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_conj(MalTypeIter begin, MalTypeIter end);
MalTypePtr mal_macro_q(MalTypeIter begin, MalTypeIter end);

struct MalCore {
    static const std::map<std::string, MalFunctionSignature> ns;
};