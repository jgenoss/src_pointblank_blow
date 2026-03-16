#pragma once

#define pp_list_reverse_impl(list)		pp_list_fold_left(pp_list_reverse_o, pp_nil, list)
#define pp_list_reverse_o(d, s, x)		(x, s)
#define pp_list_reverse_d_impl(d, list) pp_list_fold_left_ ## d(pp_list_reverse_o, pp_nil, list)

