#pragma once

#define pp_seq_filter_impl(pred, data, seq) pp_seq_tail(pp_tuple_elem(3, 2, pp_seq_fold_left(pp_seq_filter_o, (pred, data, (nil)), seq)))
#define pp_seq_filter_o(s, st, elem) pp_seq_filter_o_i(s, pp_tuple_elem(3, 0, st), pp_tuple_elem(3, 1, st), pp_tuple_elem(3, 2, st), elem)
#define pp_seq_filter_o_i(s, pred, data, res, elem) (pred, data, res pp_expr_if(pred(s, data, elem), (elem)))

#define pp_seq_filter_s_impl(s, pred, data, seq) pp_seq_tail(pp_tuple_elem(3, 2, pp_seq_fold_left_ ## s(pp_seq_filter_o, (pred, data, (nil)), seq)))
