#pragma once

#define pp_list_append_impl(a, b) pp_list_fold_right(pp_list_append_o, b, a)

#define pp_list_append_o(d, s, x) (x, s)

#define pp_list_append_d_impl(d, a, b) pp_list_fold_right_ ## d(pp_list_append_o, b, a)

