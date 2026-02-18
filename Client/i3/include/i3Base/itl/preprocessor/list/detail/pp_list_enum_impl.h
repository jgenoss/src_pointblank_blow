#pragma once

#define pp_list_enum_impl(list) pp_list_for_each_i(pp_list_enum_o, pp_nil, list)

#define pp_list_enum_o(r, _, i, elem) pp_comma_if(i) elem

#define pp_list_enum_r_impl(r, list) pp_list_for_each_i_r(r, pp_list_enum_o, pp_nil, list)
