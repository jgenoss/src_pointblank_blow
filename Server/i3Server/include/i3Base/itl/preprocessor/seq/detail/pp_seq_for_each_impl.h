#pragma once

#define pp_seq_for_each_impl(macro, data, seq) pp_for((macro, data, seq (nil)), pp_seq_for_each_p, pp_seq_for_each_o, pp_seq_for_each_m)

#define pp_seq_for_each_p(r, x) pp_dec(pp_seq_size(pp_tuple_elem(3, 2, x)))
#define pp_seq_for_each_o(r, x) pp_seq_for_each_o_i(pp_tuple_elem(3, 0, x), pp_tuple_elem(3, 1, x), pp_tuple_elem(3, 2, x))
#define pp_seq_for_each_o_i(macro, data, seq) (macro, data, pp_seq_tail(seq))
#define pp_seq_for_each_m(r, x) pp_seq_for_each_m_i(r, pp_tuple_elem(3, 0, x), pp_tuple_elem(3, 1, x), pp_tuple_elem(3, 2, x))
#define pp_seq_for_each_m_i(r, macro, data, seq) macro(r, data, pp_seq_head(seq))

#define pp_seq_for_each_r_impl(r, macro, data, seq) pp_for_ ## r((macro, data, seq (nil)), pp_seq_for_each_p, pp_seq_for_each_o, pp_seq_for_each_m)
