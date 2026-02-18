#pragma once

#define pp_list_transform_impl(op, data, list) pp_tuple_elem(3, 2, pp_list_fold_right(pp_list_transform_o, (op, data, pp_nil), list))

#define pp_list_transform_o(d, odr, elem) pp_list_transform_o_d(d, pp_tuple_elem(3, 0, odr), pp_tuple_elem(3, 1, odr), pp_tuple_elem(3, 2, odr), elem)
#define pp_list_transform_o_d(d, op, data, res, elem) (op, data, (op(d, data, elem), res))

#define pp_list_transform_d_impl(d, op, data, list) pp_tuple_elem(3, 2, pp_list_fold_right_ ## d(pp_list_transform_o, (op, data, pp_nil), list))
