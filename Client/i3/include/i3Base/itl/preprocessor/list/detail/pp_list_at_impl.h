#pragma once

#define pp_list_at_impl(list, index) pp_list_first(pp_list_rest_n(index, list))

#define pp_list_at_d_impl(d, list, index) pp_list_first(pp_list_rest_n_d(d, index, list))
