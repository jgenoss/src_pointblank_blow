#pragma once


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
