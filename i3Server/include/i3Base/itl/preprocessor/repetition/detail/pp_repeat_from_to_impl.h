#pragma once

#define pp_repeat_from_to_impl	pp_cat(pp_repeat_from_to_, pp_auto_rec(pp_repeat_p, 4))

#define pp_repeat_from_to_1(f, l, m, dt) pp_repeat_from_to_d_1(pp_auto_rec(pp_while_p, 256), f, l, m, dt)
#define pp_repeat_from_to_2(f, l, m, dt) pp_repeat_from_to_d_2(pp_auto_rec(pp_while_p, 256), f, l, m, dt)
#define pp_repeat_from_to_3(f, l, m, dt) pp_repeat_from_to_d_3(pp_auto_rec(pp_while_p, 256), f, l, m, dt)
#define pp_repeat_from_to_4(f, l, m, dt) pp_error(0x0003)


#define pp_repeat_from_to_d_impl	pp_cat(pp_repeat_from_to_d_, pp_auto_rec(pp_repeat_p, 4))
#define pp_repeat_from_to_d_1(d, f, l, m, dt) pp_repeat_1(pp_sub_d(d, l, f), pp_repeat_from_to_m_1, (d, f, m, dt))
#define pp_repeat_from_to_d_2(d, f, l, m, dt) pp_repeat_2(pp_sub_d(d, l, f), pp_repeat_from_to_m_2, (d, f, m, dt))
#define pp_repeat_from_to_d_3(d, f, l, m, dt) pp_repeat_3(pp_sub_d(d, l, f), pp_repeat_from_to_m_3, (d, f, m, dt))

#define pp_repeat_from_to_m_1(z, n, dfmd) pp_repeat_from_to_m_1_i(z, n, pp_tuple_elem(4, 0, dfmd), pp_tuple_elem(4, 1, dfmd), pp_tuple_elem(4, 2, dfmd), pp_tuple_elem(4, 3, dfmd))
#define pp_repeat_from_to_m_2(z, n, dfmd) pp_repeat_from_to_m_2_i(z, n, pp_tuple_elem(4, 0, dfmd), pp_tuple_elem(4, 1, dfmd), pp_tuple_elem(4, 2, dfmd), pp_tuple_elem(4, 3, dfmd))
#define pp_repeat_from_to_m_3(z, n, dfmd) pp_repeat_from_to_m_3_i(z, n, pp_tuple_elem(4, 0, dfmd), pp_tuple_elem(4, 1, dfmd), pp_tuple_elem(4, 2, dfmd), pp_tuple_elem(4, 3, dfmd))

#define pp_repeat_from_to_m_1_i(z, n, d, f, m, dt) pp_repeat_from_to_m_1_ii(z, pp_add_d(d, n, f), m, dt)
#define pp_repeat_from_to_m_2_i(z, n, d, f, m, dt) pp_repeat_from_to_m_2_ii(z, pp_add_d(d, n, f), m, dt)
#define pp_repeat_from_to_m_3_i(z, n, d, f, m, dt) pp_repeat_from_to_m_3_ii(z, pp_add_d(d, n, f), m, dt)

#define pp_repeat_from_to_m_1_ii(z, n, m, dt) m(z, n, dt)
#define pp_repeat_from_to_m_2_ii(z, n, m, dt) m(z, n, dt)
#define pp_repeat_from_to_m_3_ii(z, n, m, dt) m(z, n, dt)

