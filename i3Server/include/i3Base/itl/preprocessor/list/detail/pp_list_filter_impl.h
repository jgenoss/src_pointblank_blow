#pragma once

#define pp_list_filter_impl(pred, data, list) pp_tuple_elem(3, 2, pp_list_fold_right(pp_list_filter_o, (pred, data, pp_nil), list))

#define pp_list_filter_o(d, pdr, elem) pp_list_filter_o_d(d, pp_tuple_elem(3, 0, pdr), pp_tuple_elem(3, 1, pdr), pp_tuple_elem(3, 2, pdr), elem)

#define pp_list_filter_o_d(d, pred, data, res, elem) (pred, data, pp_if(pred(d, data, elem), (elem, res), res))

#define pp_list_filter_d_impl(d, pred, data, list) pp_tuple_elem(3, 2, pp_list_fold_right_ ## d(pp_list_filter_o, (pred, data, pp_nil), list))
