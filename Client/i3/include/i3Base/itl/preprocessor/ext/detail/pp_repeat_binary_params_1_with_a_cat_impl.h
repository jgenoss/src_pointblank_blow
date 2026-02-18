#pragma once

#define pp_repeat_binary_params_1_with_a_cat_impl(count, p1, p2, cat)	pp_repeat_impl(count, pp_repeat_binary_params_1_with_a_cat_m, (p1, p2, cat))
#define pp_repeat_binary_params_1_with_a_cat_m(z, n, ppc)	pp_repeat_binary_params_1_with_a_cat_m_i(z, n, pp_tuple_elem(3,0,ppc), pp_tuple_elem(3,1,ppc), pp_tuple_elem(3,2,ppc))
#define pp_repeat_binary_params_1_with_a_cat_m_i(z,n,p1,p2,cat)	pp_cat(p1, pp_inc(n))	pp_cat(pp_cat(p2, pp_inc(n)), cat)
