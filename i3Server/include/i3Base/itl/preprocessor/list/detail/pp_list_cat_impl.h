#pragma once

#define pp_list_cat_impl(list) pp_list_fold_left(pp_list_cat_o, pp_list_first(list), pp_list_rest(list))

#define pp_list_cat_o(d, s, x) pp_cat(s, x)

#define pp_list_cat_d_impl(d, list) pp_list_fold_left_ ## d(pp_list_cat_o, pp_list_first(list), pp_list_rest(list))
