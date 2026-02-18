#pragma once

#define pp_seq_for_each_i_impl(macro, data, seq) pp_for((macro, data, seq (nil), 0), pp_seq_for_each_i_p, pp_seq_for_each_i_o, pp_seq_for_each_i_m)

#define pp_seq_for_each_i_p(r, x) pp_dec(pp_seq_size(pp_tuple_elem(4, 2, x)))
#define pp_seq_for_each_i_o(r, x) pp_seq_for_each_i_o_i(pp_tuple_elem(4, 0, x), pp_tuple_elem(4, 1, x), pp_tuple_elem(4, 2, x), pp_tuple_elem(4, 3, x))
#define pp_seq_for_each_i_o_i(macro, data, seq, i) (macro, data, pp_seq_tail(seq), pp_inc(i))
#define pp_seq_for_each_i_m(r, x) pp_seq_for_each_i_m_i(r, pp_tuple_elem(4, 0, x), pp_tuple_elem(4, 1, x), pp_tuple_elem(4, 2, x), pp_tuple_elem(4, 3, x))
#define pp_seq_for_each_i_m_i(r, macro, data, seq, i) macro(r, data, i, pp_seq_head(seq))

#define pp_seq_for_each_i_r_impl(r, macro, data, seq) pp_for_ ## r((macro, data, seq (nil), 0), pp_seq_for_each_i_p, pp_seq_for_each_i_o, pp_seq_for_each_i_m)
