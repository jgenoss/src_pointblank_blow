#pragma once

#define pp_enum_params_cat_params_impl(count, p1, cat, p2)	pp_repeat_impl(count, pp_enum_params_cat_params_m, (p1, cat, p2) )
#define pp_enum_params_cat_params_m(z, n, pcp)		pp_enum_params_cat_params_m_i(z, n, pp_tuple_elem(3, 0, pcp), pp_tuple_elem(3, 1, pcp), pp_tuple_elem(3, 2, pcp))
#define pp_enum_params_cat_params_m_i(z, n, p1, c, p2)		pp_comma_if(n) pp_cat(pp_cat(p1, n),c)	pp_cat(p2, n)
