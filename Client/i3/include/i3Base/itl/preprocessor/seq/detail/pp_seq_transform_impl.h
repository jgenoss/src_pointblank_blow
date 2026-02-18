#pragma once

#define pp_seq_transform_impl(op, data, seq) pp_seq_tail(pp_tuple_elem(3, 2, pp_seq_fold_left(pp_seq_transform_o, (op, data, (nil)), seq)))

#define pp_seq_transform_o(s, state, elem) pp_seq_transform_o_i(s, pp_tuple_elem(3, 0, state), pp_tuple_elem(3, 1, state), pp_tuple_elem(3, 2, state), elem)
#define pp_seq_transform_o_i(s, op, data, res, elem) (op, data, res (op(s, data, elem)))

#define pp_seq_transform_s_impl(s, op, data, seq) pp_seq_tail(pp_tuple_elem(3, 2, pp_seq_fold_left_ ## s(pp_seq_transform_o, (op, data, (nil)), seq)))
