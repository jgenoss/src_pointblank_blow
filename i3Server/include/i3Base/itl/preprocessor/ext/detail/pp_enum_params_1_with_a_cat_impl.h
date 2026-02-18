#pragma once

#define pp_enum_params_1_with_a_cat_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_params_1_with_a_cat_m, (p1, p2) )
#define pp_enum_params_1_with_a_cat_m(z, n, pp)			pp_enum_params_1_with_a_cat_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_params_1_with_a_cat_m_i(z, n, p1, p2)	pp_comma_if(n)	pp_cat( pp_cat(p1, pp_inc(n)), p2 )
