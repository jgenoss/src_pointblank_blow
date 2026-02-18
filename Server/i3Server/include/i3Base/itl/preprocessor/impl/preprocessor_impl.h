#pragma once



#define pp_cat_impl(a, b)	pp_cat_impl0(a ## b)
#define pp_cat_impl0(res)	res

#define pp_expand_impl(x)	pp_expand_i(x)
#define pp_expand_i(x)		x

#include "logical/pp_bool_impl.h"


#define pp_iif_impl_res0(t, f)	f 
#define pp_iif_impl_res1(t, f)	t
#define pp_iif_impl0(res)		res
#define pp_iif_impl( bit, t, f )	pp_iif_impl0(pp_iif_impl_res ## bit(t, f))

#define pp_if_impl(cond, t, f)		pp_iif( pp_bool(cond), t, f)

#define pp_expr_iif_impl(bit, expr) pp_expr_iif_i(bit, expr)
#define pp_expr_iif_i(bit, expr)	pp_expr_iif_ ## bit(expr)
#define pp_expr_iif_0(expr)
#define pp_expr_iif_1(expr)		expr

#define pp_expr_if_impl(cond, expr) pp_expr_iif(pp_bool(cond), expr)

#define pp_bitand_impl(x, y)		pp_bitand_i(x, y)
#define pp_bitand_i(x, y)			pp_bitand_id(pp_bitand_ ## x ## y)
#define pp_bitand_id(res)			res
#define pp_bitand_00 0
#define pp_bitand_01 0
#define pp_bitand_10 0
#define pp_bitand_11 1

#define pp_and_impl(p, q)			pp_bitand(pp_bool(p), pp_bool(q))


#define pp_empty_impl()
#define pp_comma_impl()	,
#define pp_identity_impl(item)		item pp_empty
#define pp_comma_if_impl(cond)		pp_if(cond, pp_comma, pp_empty)()


#include "detail/pp_auto_rec_impl.h"
#include "tuple/pp_tuple_eat_impl.h"

#define pp_error_impl(code)				pp_cat(pp_error_, code)

# define pp_error_0x0000 pp_error(0x0000, PP_INDEX_OUT_OF_BOUNDS)
# define pp_error_0x0001 pp_error(0x0001, PP_WHILE_OVERFLOW)
# define pp_error_0x0002 pp_error(0x0002, PP_FOR_OVERFLOW)
# define pp_error_0x0003 pp_error(0x0003, PP_REPEAT_OVERFLOW)
# define pp_error_0x0004 pp_error(0x0004, PP_LIST_FOLD_OVERFLOW)
# define pp_error_0x0005 pp_error(0x0005, PP_SEQ_FOLD_OVERFLOW)
# define pp_error_0x0006 pp_error(0x0006, PP_ARITHMETIC_OVERFLOW)
# define pp_error_0x0007 pp_error(0x0007, PP_DIVISION_BY_ZERO)

#include "repetition/pp_repeat_impl.h"

#define pp_enum_impl					pp_cat(pp_enum_, pp_auto_rec(pp_repeat_p, 4))
#define pp_enum_1(c, m, d)				pp_repeat_1(c, pp_enum_m_1, (m, d))
#define pp_enum_2(c, m, d)				pp_repeat_2(c, pp_enum_m_2, (m, d))
#define pp_enum_3(c, m, d)				pp_repeat_3(c, pp_enum_m_3, (m, d))
#define pp_enum_4(c, m, d)				pp_error(0x0003)

#define pp_enum_m_1(z, n, md)			pp_enum_m_1_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_m_2(z, n, md)			pp_enum_m_2_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_m_3(z, n, md)			pp_enum_m_3_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))

# define pp_enum_m_1_i(z, n, m, d)		pp_comma_if(n) m(z, n, d)
# define pp_enum_m_2_i(z, n, m, d)		pp_comma_if(n) m(z, n, d)
# define pp_enum_m_3_i(z, n, m, d)		pp_comma_if(n) m(z, n, d)

/////
#define pp_enum_trailing_impl			pp_cat(pp_enum_trailing_, pp_auto_rec(pp_repeat_p, 4))
#define pp_enum_trailing_1(c, m, d)		pp_repeat_1(c, pp_enum_trailing_m_1, (m, d))
#define pp_enum_trailing_2(c, m, d)		pp_repeat_2(c, pp_enum_trailing_m_2, (m, d))
#define pp_enum_trailing_3(c, m, d)		pp_repeat_3(c, pp_enum_trailing_m_3, (m, d))
#define pp_enum_trailing_4(c, m, d)		pp_error(0x0003)

#define pp_enum_trailing_m_1(z, n, md)	pp_enum_trailing_m_1_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_trailing_m_2(z, n, md)	pp_enum_trailing_m_2_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_trailing_m_3(z, n, md)	pp_enum_trailing_m_3_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))

#define pp_enum_trailing_m_1_i(z, n, m, d) , m(z, n, d)
#define pp_enum_trailing_m_2_i(z, n, m, d) , m(z, n, d)
#define pp_enum_trailing_m_3_i(z, n, m, d) , m(z, n, d)
/////

#define pp_enum_shifted_impl			pp_cat(pp_enum_shifted_, pp_auto_rec(pp_repeat_p, 4))
#define pp_enum_shifted_1(c, m, d)		pp_repeat_1(pp_dec(c), pp_enum_shifted_m_1, (m, d))
#define pp_enum_shifted_2(c, m, d)		pp_repeat_2(pp_dec(c), pp_enum_shifted_m_2, (m, d))
#define pp_enum_shifted_3(c, m, d)		pp_repeat_3(pp_dec(c), pp_enum_shifted_m_3, (m, d))
#define pp_enum_shifted_4(c, m, d)		pp_error(0x0003)

#define pp_enum_shifted_m_1(z, n, md)	pp_enum_shifted_m_1_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_shifted_m_2(z, n, md)	pp_enum_shifted_m_2_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_shifted_m_3(z, n, md)	pp_enum_shifted_m_3_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))

#define pp_enum_shifted_m_1_i(z, n, m, d) pp_comma_if(n) m(z, pp_inc(n), d)
#define pp_enum_shifted_m_2_i(z, n, m, d) pp_comma_if(n) m(z, pp_inc(n), d)
#define pp_enum_shifted_m_3_i(z, n, m, d) pp_comma_if(n) m(z, pp_inc(n), d)



#define pp_enum_params_impl(count, param)			pp_repeat_impl(count, pp_enum_params_m, param)
#define pp_enum_params_m(z, n, param)				pp_comma_if(n)	param ## n
#define pp_enum_params_z_impl(z, count, param)		pp_repeat_ ## z(count, pp_enum_params_m, param)


//
#define pp_enum_params_1_impl(count, param)			pp_repeat_impl(count, pp_enum_params_1_m, param)
#define pp_enum_params_1_m(z, n, param)				pp_comma_if(n)  pp_cat(param, pp_inc(n))
//
#define pp_repeat_params_impl(count, param)			pp_repeat_impl(count, pp_repeat_params_m, param)
#define pp_repeat_params_m(z, n, param)				pp_cat(param, n)

#define pp_repeat_params_1_impl(count, param)		pp_repeat_impl(count, pp_repeat_params_1_m, param)
#define pp_repeat_params_1_m(z, n, param)			pp_cat(param, pp_inc(n))

#define pp_repeat_binary_params_impl(count, p1, p2)	pp_repeat_impl(count, pp_repeat_binary_params_m, (p1, p2))
#define pp_repeat_binary_params_m(z, n, pp)			pp_repeat_binary_params_m_i(z, n, pp_tuple_elem(2,0,pp), pp_tuple_elem(2,1,pp))
#define pp_repeat_binary_params_m_i(z, n, p1, p2)	pp_cat(p1, n)	pp_cat(p2, n)

#define pp_repeat_binary_params_1_impl(count, p1, p2)	pp_repeat_impl(count, pp_repeat_binary_params_1_m, (p1, p2))
#define pp_repeat_binary_params_1_m(z, n, pp)			pp_repeat_binary_params_1_m_i(z, n, pp_tuple_elem(2,0,pp), pp_tuple_elem(2,1,pp))
#define pp_repeat_binary_params_1_m_i(z, n, p1, p2)		pp_cat(p1, pp_inc(n))	pp_cat(p2, pp_inc(n))

#define pp_repeat_binary_params_with_a_cat_impl(count, p1, p2, cat)		pp_repeat_impl(count, pp_repeat_binary_params_with_a_cat_m, (p1, p2, cat))
#define pp_repeat_binary_params_with_a_cat_m(z, n, ppc)	pp_repeat_binary_params_with_a_cat_m_i(z, n, pp_tuple_elem(3,0,ppc), pp_tuple_elem(3,1,ppc), pp_tuple_elem(3,2,ppc))
#define pp_repeat_binary_params_with_a_cat_m_i(z,n,p1,p2,cat)	pp_cat(p1, n)	pp_cat(pp_cat(p2, n), cat)

#define pp_repeat_binary_params_1_with_a_cat_impl(count, p1, p2, cat)	pp_repeat_impl(count, pp_repeat_binary_params_1_with_a_cat_m, (p1, p2, cat))
#define pp_repeat_binary_params_1_with_a_cat_m(z, n, ppc)	pp_repeat_binary_params_1_with_a_cat_m_i(z, n, pp_tuple_elem(3,0,ppc), pp_tuple_elem(3,1,ppc), pp_tuple_elem(3,2,ppc))
#define pp_repeat_binary_params_1_with_a_cat_m_i(z,n,p1,p2,cat)	pp_cat(p1, pp_inc(n))	pp_cat(pp_cat(p2, pp_inc(n)), cat)


#define pp_enum_trailing_params_impl(count, param)	pp_repeat_impl(count, pp_enum_trailing_params_m, param)
#define pp_enum_trailing_params_m(z, n, param)		, param ## n
#define pp_enum_trailing_params_z_impl(z, count, param) pp_repeat_ ## z(count, pp_enum_trailing_params_m, param)

#define pp_enum_shifted_params_impl(count, param)	pp_repeat_impl(pp_dec(count), pp_enum_shifted_params_m, param)
#define pp_enum_shifted_params_m(z, n, param)		pp_comma_if(n) pp_cat(param, pp_inc(n))
#define pp_enum_shifted_params_z_impl(z, count, param) pp_repeat_ ## z(pp_dec(count), pp_enum_shifted_params_m, param)

#define pp_enum_trailing_params_1_impl(count, param)	pp_repeat_impl(count, pp_enum_trailing_params_1_m, param)
#define pp_enum_trailing_params_1_m(z, n, param)	, pp_cat(param, pp_inc(n))

#define pp_enum_shifted_params_1_impl(count, param) pp_repeat_impl(pp_dec(count), pp_enum_shifted_params_1_m, param)
#define pp_enum_shifted_params_1_m(z, n, param)		pp_comma_if(n) pp_cat(param, pp_add(n, 2))



#include "facilities/pp_intercept_impl.h"

#define pp_enum_binary_params_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_binary_params_m, (p1, p2) )
#define pp_enum_binary_params_m(z, n, pp)			pp_enum_binary_params_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_binary_params_m_i(z, n, p1, p2)		pp_comma_if(n) pp_cat(p1, n)	pp_cat(p2, n)
#define pp_enum_binary_params_z_impl(z, count, p1, p2) pp_repeat_ ## z(count, pp_enum_binary_params_m, (p1, p2) )

#define pp_enum_binary_params_1_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_binary_params_1_m, (p1, p2) )
#define pp_enum_binary_params_1_m(z, n, pp)			pp_enum_binary_params_1_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_binary_params_1_m_i(z, n, p1, p2)	pp_comma_if(n) pp_cat(p1, pp_inc(n))  pp_cat(p2, pp_inc(n))


#define pp_enum_params_cat_params_impl(count, p1, cat, p2)	pp_repeat_impl(count, pp_enum_params_cat_params_m, (p1, cat, p2) )
#define pp_enum_params_cat_params_m(z, n, pcp)		pp_enum_params_cat_params_m_i(z, n, pp_tuple_elem(3, 0, pcp), pp_tuple_elem(3, 1, pcp), pp_tuple_elem(3, 2, pcp))
#define pp_enum_params_cat_params_m_i(z, n, p1, c, p2)		pp_comma_if(n) pp_cat(pp_cat(p1, n),c)	pp_cat(p2, n)

#define pp_enum_params_1_cat_params_impl(count, p1, cat, p2) pp_repeat_impl(count, pp_enum_params_1_cat_params_m, (p1, cat, p2) )	
#define pp_enum_params_1_cat_params_m(z, n, pcp)	pp_enum_params_1_cat_params_m_i(z, n, pp_tuple_elem(3, 0, pcp), pp_tuple_elem(3, 1, pcp), pp_tuple_elem(3, 2, pcp))
#define pp_enum_params_1_cat_params_m_i(z, n, p1, c, p2)	pp_comma_if(n) pp_cat(pp_cat(p1, pp_inc(n)),c)	pp_cat(p2, pp_inc(n))


#define pp_enum_params_with_a_cat_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_params_with_a_cat_m, (p1, p2) )
#define pp_enum_params_with_a_cat_m(z, n, pp)			pp_enum_params_with_a_cat_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_params_with_a_cat_m_i(z, n, p1, p2)		pp_comma_if(n)	pp_cat( pp_cat(p1, n), p2 )

#define pp_enum_params_1_with_a_cat_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_params_1_with_a_cat_m, (p1, p2) )
#define pp_enum_params_1_with_a_cat_m(z, n, pp)			pp_enum_params_1_with_a_cat_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_params_1_with_a_cat_m_i(z, n, p1, p2)	pp_comma_if(n)	pp_cat( pp_cat(p1, pp_inc(n)), p2 )


#define pp_enum_trailing_binary_params_impl(count, p1, p2)	pp_repeat_impl(count, pp_enum_trailing_binary_params_m, (p1, p2))
#define pp_enum_trailing_binary_params_m(z, n, pp)			pp_enum_trailing_binary_params_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_trailing_binary_params_m_i(z, n, p1, p2)	, pp_cat(p1, n) pp_cat(p2, n)
#define pp_enum_trailing_binary_params_z_impl(z, count, p1, p2)	pp_repeat_ ## z(count, pp_enum_trailing_binary_params_m, (p1, p2))

#define pp_enum_shifted_binary_params_impl(count, p1, p2)	pp_repeat_impl(pp_dec(count), pp_enum_shifted_binary_params_m, (p1, p2))
#define pp_enum_shifted_binary_params_m(z, n, pp)			pp_enum_shifted_binary_params_m_i(z, n, pp_tuple_elem(2, 0, pp), pp_tuple_elem(2, 1, pp))
#define pp_enum_shifted_binary_params_m_i(z, n, p1, p2)		pp_comma_if(n) pp_cat(p1, pp_inc(n)) pp_cat(p2, pp_inc(n))
#define pp_enum_shifted_binary_params_z_impl(z, count, p1, p2)	pp_repeat_ ## z(pp_dec(count), pp_enum_shifted_binary_params_m, (p1, p2))

// tuple/elem 구현..
#include "tuple/pp_tuple_elem_impl.h"

#define pp_enum_params_with_a_default_impl(count, param, def)	pp_enum_binary_params(count, param, = def pp_intercept)
#define pp_enum_params_1_with_a_default_impl(count, param, def)	pp_enum_binary_params_1(count, param, = def pp_intercept)

#define pp_enum_params_with_defaults_impl(count, param, def)	pp_enum_binary_params(count, param, = def)
#define pp_enum_params_1_with_defaults_impl(count, param, def)	pp_enum_binary_params_1(count, param, = def)

#include "arithmetic/pp_inc_impl.h"
#include "arithmetic/pp_dec_impl.h"

#define pp_array_data_impl(array)					pp_tuple_elem(2, 1, array)
#define pp_array_elem_impl(i, array)				pp_tuple_elem(pp_array_size(array), i, pp_array_data(array))
#define pp_array_size_impl(array)					pp_tuple_elem(2, 0, array)


#define pp_stringize_impl(text)			pp_stringize_a((text))
#define pp_stringize_a(arg)				pp_stringize_i arg
#define pp_stringize_i(text)			#text

#define pp_wstringize_impl(text)		pp_wstringize_i(text)
#define pp_wstringize_i(text)			pp_wstringize_ii(#text)
#define pp_wstringize_ii(str)			L ## str


#include "detail/pp_check.h"

#define pp_is_binary(x)					pp_check(x, pp_is_binary_check)
#define pp_is_binary_check(a, b)		1
#define pp_check_result_pp_is_binary_check 0, pp_nil

#define pp_compl_impl(x)				pp_compl_i(x)
#define pp_compl_i(x)					pp_compl_id(pp_compl_ ## x)
#define pp_compl_id(id)					id
#define pp_compl_0 1
#define pp_compl_1 0

#define pp_not_impl(x)					pp_compl(pp_bool(x))

#include "list/pp_list_adt.h"

#include "list/pp_list_fold_left_impl.h"

#define pp_list_reverse_impl(list)		pp_list_fold_left(pp_list_reverse_o, pp_nil, list)
#define pp_list_reverse_o(d, s, x)		(x, s)
#define pp_list_reverse_d_impl(d, list) pp_list_fold_left_ ## d(pp_list_reverse_o, pp_nil, list)

#include "list/pp_list_fold_right_impl.h"

#include "control/pp_while_impl.h"

#define pp_add_impl(x, y)				pp_tuple_elem(2, 0, pp_while(pp_add_p, pp_add_o, (x, y)))
#define pp_add_p(d, xy)					pp_tuple_elem(2, 1, xy)
#define pp_add_o(d, xy)					pp_add_o_i xy
#define pp_add_o_i(x, y)				(pp_inc(x), pp_dec(y))
#define pp_add_d_impl(d, x, y)			pp_tuple_elem(2, 0, pp_while_ ## d(pp_add_p, pp_add_o, (x, y)))


#define pp_sub_impl(x, y)				pp_tuple_elem(2, 0, pp_while(pp_sub_p, pp_sub_o, (x, y)))
#define pp_sub_p(d, xy)					pp_tuple_elem(2, 1, xy)
#define pp_sub_o(d, xy)					pp_sub_o_i xy
#define pp_sub_o_i(x, y)				(pp_dec(x), pp_dec(y))
#define pp_sub_d_impl(d, x, y)			pp_tuple_elem(2, 0, pp_while_ ## d(pp_sub_p, pp_sub_o, (x, y)))

#include "tuple/pp_tuple_rem_impl.h"

#define pp_mul_impl(x, y)				pp_tuple_elem(3, 0, pp_while(pp_mul_p, pp_mul_o, (0, x, y)))
#define pp_mul_p(d, rxy)				pp_tuple_elem(3, 2, rxy)
#define pp_mul_o(d, rxy)				pp_mul_o_i(d, pp_tuple_elem(3, 0, rxy), pp_tuple_elem(3, 1, rxy), pp_tuple_elem(3, 2, rxy))
#define pp_mul_o_i(d, r, x, y)			(pp_add_d(d, r, x), x, pp_dec(y))
#define pp_mul_d_impl(d, x, y)			pp_tuple_elem(3, 0, pp_while_ ## d(pp_mul_p, pp_mul_o, (0, x, y)))

#include "comparison/pp_not_equal_impl.h"
#define pp_equal_impl(x, y)				pp_compl(pp_not_equal(x, y))


#define pp_less_equal_impl(x, y)		pp_not(pp_sub(x, y))
#define pp_less_equal_d_impl(d, x, y)	pp_not(pp_sub_d(d, x, y))


#define pp_less_impl(x, y)				pp_iif(pp_not_equal(x, y), pp_less_equal, 0 pp_tuple_eat_2)(x, y)
#define pp_less_d_impl(d, x, y)			pp_iif(pp_not_equal(x, y), pp_less_equal_d, 0 pp_tuple_eat_3)(d, x, y)

#define pp_greater_equal_impl(x, y)			pp_less_equal(y, x)
#define pp_greater_equal_d_impl(d, x, y)	pp_less_equal_d(d, y, x)

#define pp_greater_impl(x, y)			pp_less(y, x)
#define pp_greater_d_impl(d, x, y)		pp_less_d(d, y, x)

#define pp_div_base(x, y)			pp_while(pp_div_base_p, pp_div_base_o, (0, x, y))
#define pp_div_base_p(d, rxy)		pp_div_base_p_i(d, pp_tuple_elem(3, 0, rxy), pp_tuple_elem(3, 1, rxy), pp_tuple_elem(3, 2, rxy))
#define pp_div_base_p_i(d, r, x, y) pp_less_equal_d(d, y, x)
#define pp_div_base_o(d, rxy)		pp_div_base_o_i(d, pp_tuple_elem(3, 0, rxy), pp_tuple_elem(3, 1, rxy), pp_tuple_elem(3, 2, rxy))
#define pp_div_base_o_i(d, r, x, y) (pp_inc(r), pp_sub_d(d, x, y), y)
#define pp_div_base_d(d, x, y)		pp_while_ ## d(pp_div_base_p, pp_div_base_o, (0, x, y))

#define pp_div_impl(x, y)			pp_tuple_elem(3, 0, pp_div_base(x, y))
#define pp_div_d_impl(d, x, y)		pp_tuple_elem(3, 0, pp_div_base_d(d, x, y))

#define pp_mod_impl(x, y)			pp_tuple_elem(3, 1, pp_div_base(x, y))
#define pp_mod_d_impl(d, x, y)		pp_tuple_elem(3, 1, pp_div_base_d(d, x, y))

#include "repetition/pp_repeat_from_to_impl.h"
///// ext버전

#define pp_enum_datas_impl(count, data)				pp_repeat_impl(count, pp_enum_datas_m, data)
#define pp_enum_datas_m(z, n, data)					pp_comma_if(n)	data


#define pp_typename_impl()							typename 
#define pp_typename_if_impl(cond)					pp_if(cond, pp_typename, pp_empty)()

#define pp_enum_params_from_to_impl(first, last, param)			pp_repeat_from_to(first, last, pp_enum_params_from_to_m, (first, param) )
#define pp_enum_params_from_to_m(z, n, fp)			pp_enum_params_from_to_m_i(z, n, pp_tuple_elem(2, 0, fp), pp_tuple_elem(2, 1, fp))
#define pp_enum_params_from_to_m_i(z, n, f, p)		pp_comma_if(pp_not_equal(n, f)) pp_cat(p, n)


#define pp_enum_binary_params_from_to_impl(first, last, p1, p2)	pp_repeat_from_to(first, last, pp_enum_binary_params_from_to_m, (first, p1, p2) )
#define pp_enum_binary_params_from_to_m(z, n, fpp) pp_enum_binary_params_from_to_m_i(z, n, pp_tuple_elem(3, 0, fpp), pp_tuple_elem(3, 1, fpp), pp_tuple_elem(3, 2, fpp))
#define pp_enum_binary_params_from_to_m_i(z, n, f, p1, p2) pp_comma_if(pp_not_equal(n, f)) pp_cat(p1, n)	pp_cat(p2, n)


#define pp_enum_params_from_to_with_a_default_impl(first, last, param, def)	pp_enum_binary_params_from_to(first, last, param, = def pp_intercept)
#define pp_enum_params_from_to_with_defaults_impl(first, last, param, def)	pp_enum_binary_params_from_to(first, last, param, = def)

#define pp_enum_params_1_from_to_impl(first, last, param)		pp_repeat_from_to(first, last, pp_enum_params_1_from_to_m, (first, param) )
#define pp_enum_params_1_from_to_m(z, n, fp)			pp_enum_params_1_from_to_m_i(z, n, pp_tuple_elem(2, 0, fp), pp_tuple_elem(2, 1, fp))
#define pp_enum_params_1_from_to_m_i(z, n, f, p)		pp_comma_if(pp_not_equal(n, f)) pp_cat(p, pp_inc(n))


#define pp_enum_binary_params_1_from_to_impl(first, last, p1, p2)	pp_repeat_from_to(first, last, pp_enum_binary_params_1_from_to_m, (first, p1, p2) )
#define pp_enum_binary_params_1_from_to_m(z, n, fpp) pp_enum_binary_params_1_from_to_m_i(z, n, pp_tuple_elem(3, 0, fpp), pp_tuple_elem(3, 1, fpp), pp_tuple_elem(3, 2, fpp))
#define pp_enum_binary_params_1_from_to_m_i(z, n, f, p1, p2) pp_comma_if(pp_not_equal(n, f)) pp_cat(p1, pp_inc(n))	pp_cat(p2, pp_inc(n))


#define pp_enum_params_1_from_to_with_a_default_impl(first, last, param, def)	pp_enum_binary_params_1_from_to(first, last, param, = def pp_intercept)
#define pp_enum_params_1_from_to_with_defaults_impl(first, last, param, def)	pp_enum_binary_params_1_from_to(first, last, param, = def)

#define pp_enum_params_spec_impl(total_count, param_count, param, def_param) pp_enum_params(param_count, param) pp_comma_if(pp_and(param_count, pp_sub(total_count, param_count))) pp_enum_datas(pp_sub(total_count, param_count), def_param)
#define pp_enum_params_1_spec_impl(total_count, param_count, param, def_param) pp_enum_params_1(param_count, param) pp_comma_if(pp_and(param_count, pp_sub(total_count, param_count))) pp_enum_datas(pp_sub(total_count, param_count), def_param)

#define pp_enum_params_with_a_partial_default_impl(total_count, no_def_count, param, def) pp_enum_params(no_def_count, param) pp_comma_if(pp_and(no_def_count, pp_sub(total_count, no_def_count))) \
	pp_enum_params_from_to_with_a_default(no_def_count, total_count, param, def)
	
#define pp_enum_params_1_with_a_partial_default_impl(total_count, no_def_count, param, def) pp_enum_params_1(no_def_count, param) pp_comma_if(pp_and(no_def_count, pp_sub(total_count, no_def_count))) \
	pp_enum_params_1_from_to_with_a_default(no_def_count, total_count, param, def)


