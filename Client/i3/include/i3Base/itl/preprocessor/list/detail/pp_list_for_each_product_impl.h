#pragma once

#define pp_list_for_each_product_impl(macro, size, tuple) pp_list_for_each_product_e(pp_for, macro, size, pp_tuple_to_list(size, tuple))

#define pp_list_for_each_product_r_impl(r, macro, size, tuple) pp_list_for_each_product_e(pp_for_ ## r, macro, size, pp_tuple_to_list(size, tuple))


#define pp_list_for_each_product_e(impl, macro, size, lists) impl((pp_list_first(lists), pp_list_rest(lists), pp_nil, macro, size), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_0)

#define pp_list_for_each_product_p(r, data) pp_list_is_cons(pp_tuple_elem(5, 0, data))

#define pp_list_for_each_product_o(r, data) pp_list_for_each_product_o_i data
#define pp_list_for_each_product_o_i(a, b, res, macro, size) (pp_list_rest(a), b, res, macro, size)

#define pp_list_for_each_product_i(r, data) pp_list_for_each_product_i_i(r, pp_tuple_elem(5, 0, data), pp_tuple_elem(5, 1, data), pp_tuple_elem(5, 2, data), pp_tuple_elem(5, 3, data), pp_tuple_elem(5, 4, data))


#define pp_list_for_each_product_i_i(r, a, b, res, macro, size) pp_list_for_each_product_i_ii(r, macro, pp_list_to_tuple_r(r, (pp_list_first(a), res)), size)
#define pp_list_for_each_product_i_ii(r, macro, args, size) pp_list_for_each_product_i_iii(r, macro, args, size)
#define pp_list_for_each_product_i_iii(r, macro, args, size) macro(r, pp_tuple_reverse(size, args))

#define pp_list_for_each_product_c(data, i) pp_if(pp_list_is_cons(pp_tuple_elem(5, 1, data)), pp_list_for_each_product_n_ ## i, pp_list_for_each_product_i)

#define pp_list_for_each_product_h(data) pp_list_for_each_product_h_i data

#define pp_list_for_each_product_h_i(a, b, res, macro, size) (pp_list_first(b), pp_list_rest(b), (pp_list_first(a), res), macro, size)


# define pp_list_for_each_product_m_0(r, data) pp_list_for_each_product_c(data, 0)(r, data)
# define pp_list_for_each_product_m_1(r, data) pp_list_for_each_product_c(data, 1)(r, data)
# define pp_list_for_each_product_m_2(r, data) pp_list_for_each_product_c(data, 2)(r, data)
# define pp_list_for_each_product_m_3(r, data) pp_list_for_each_product_c(data, 3)(r, data)
# define pp_list_for_each_product_m_4(r, data) pp_list_for_each_product_c(data, 4)(r, data)
# define pp_list_for_each_product_m_5(r, data) pp_list_for_each_product_c(data, 5)(r, data)
# define pp_list_for_each_product_m_6(r, data) pp_list_for_each_product_c(data, 6)(r, data)
# define pp_list_for_each_product_m_7(r, data) pp_list_for_each_product_c(data, 7)(r, data)
# define pp_list_for_each_product_m_8(r, data) pp_list_for_each_product_c(data, 8)(r, data)
# define pp_list_for_each_product_m_9(r, data) pp_list_for_each_product_c(data, 9)(r, data)
# define pp_list_for_each_product_m_10(r, data) pp_list_for_each_product_c(data, 10)(r, data)
# define pp_list_for_each_product_m_11(r, data) pp_list_for_each_product_c(data, 11)(r, data)
# define pp_list_for_each_product_m_12(r, data) pp_list_for_each_product_c(data, 12)(r, data)
# define pp_list_for_each_product_m_13(r, data) pp_list_for_each_product_c(data, 13)(r, data)
# define pp_list_for_each_product_m_14(r, data) pp_list_for_each_product_c(data, 14)(r, data)
# define pp_list_for_each_product_m_15(r, data) pp_list_for_each_product_c(data, 15)(r, data)
# define pp_list_for_each_product_m_16(r, data) pp_list_for_each_product_c(data, 16)(r, data)
# define pp_list_for_each_product_m_17(r, data) pp_list_for_each_product_c(data, 17)(r, data)
# define pp_list_for_each_product_m_18(r, data) pp_list_for_each_product_c(data, 18)(r, data)
# define pp_list_for_each_product_m_19(r, data) pp_list_for_each_product_c(data, 19)(r, data)
# define pp_list_for_each_product_m_20(r, data) pp_list_for_each_product_c(data, 20)(r, data)
# define pp_list_for_each_product_m_21(r, data) pp_list_for_each_product_c(data, 21)(r, data)
# define pp_list_for_each_product_m_22(r, data) pp_list_for_each_product_c(data, 22)(r, data)
# define pp_list_for_each_product_m_23(r, data) pp_list_for_each_product_c(data, 23)(r, data)
# define pp_list_for_each_product_m_24(r, data) pp_list_for_each_product_c(data, 24)(r, data)
# define pp_list_for_each_product_m_25(r, data) pp_list_for_each_product_c(data, 25)(r, data)
#
# define pp_list_for_each_product_n_0(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_1)
# define pp_list_for_each_product_n_1(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_2)
# define pp_list_for_each_product_n_2(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_3)
# define pp_list_for_each_product_n_3(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_4)
# define pp_list_for_each_product_n_4(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_5)
# define pp_list_for_each_product_n_5(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_6)
# define pp_list_for_each_product_n_6(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_7)
# define pp_list_for_each_product_n_7(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_8)
# define pp_list_for_each_product_n_8(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_9)
# define pp_list_for_each_product_n_9(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_10)
# define pp_list_for_each_product_n_10(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_11)
# define pp_list_for_each_product_n_11(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_12)
# define pp_list_for_each_product_n_12(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_13)
# define pp_list_for_each_product_n_13(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_14)
# define pp_list_for_each_product_n_14(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_15)
# define pp_list_for_each_product_n_15(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_16)
# define pp_list_for_each_product_n_16(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_17)
# define pp_list_for_each_product_n_17(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_18)
# define pp_list_for_each_product_n_18(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_19)
# define pp_list_for_each_product_n_19(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_20)
# define pp_list_for_each_product_n_20(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_21)
# define pp_list_for_each_product_n_21(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_22)
# define pp_list_for_each_product_n_22(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_23)
# define pp_list_for_each_product_n_23(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_24)
# define pp_list_for_each_product_n_24(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_25)
# define pp_list_for_each_product_n_25(r, data) pp_for_ ## r(pp_list_for_each_product_h(data), pp_list_for_each_product_p, pp_list_for_each_product_o, pp_list_for_each_product_m_26)
