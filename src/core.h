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

#include "env.h"
#include "types.h"
#include <map>
#include <string>

RalTypePtr ral_add(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_sub(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_mul(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_div(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_list(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_list_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_empty_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_count(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_equal(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_lt(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_le(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_gt(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_ge(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_pr_str(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_str(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_prn(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_println(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_read_string(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_slurp(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_atom(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_atom_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_deref(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_reset(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_swap(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_cons(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_concat(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_nth(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_first(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_rest(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_throw(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_apply(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_map(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_nil_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_true_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_false_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_symbol_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_symbol(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_keyword(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_keyword_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_vector(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_vector_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_sequential_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_hash_map(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_map_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_assoc(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_dissoc(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_get(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_contains_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_keys(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_vals(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_readline(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_time_ms(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_meta(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_with_meta(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_fn_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_string_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_number_q(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_seq(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_conj(RalTypeIter begin, RalTypeIter end);
RalTypePtr ral_macro_q(RalTypeIter begin, RalTypeIter end);

struct RalCore {
    static const std::map<std::string, RalFunctionSignature> ns;
};