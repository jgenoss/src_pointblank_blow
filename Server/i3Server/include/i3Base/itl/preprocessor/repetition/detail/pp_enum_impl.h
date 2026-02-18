#pragma once

#define pp_enum_impl					pp_cat(pp_enum_, pp_auto_rec(pp_repeat_p, 4))
#define pp_enum_1(c, m, d)				pp_repeat_1(c, pp_enum_m_1, (m, d))
#define pp_enum_2(c, m, d)				pp_repeat_2(c, pp_enum_m_2, (m, d))
#define pp_enum_3(c, m, d)				pp_repeat_3(c, pp_enum_m_3, (m, d))
#define pp_enum_4(c, m, d)				pp_error(0x0003)

#define pp_enum_m_1(z, n, md)			pp_enum_m_1_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_m_2(z, n, md)			pp_enum_m_2_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))
#define pp_enum_m_3(z, n, md)			pp_enum_m_3_i(z, n, pp_tuple_elem(2, 0, md), pp_tuple_elem(2, 1, md))

#define pp_enum_m_1_i(z, n, m, d)		pp_comma_if(n) m(z, n, d)
#define pp_enum_m_2_i(z, n, m, d)		pp_comma_if(n) m(z, n, d)
#define pp_enum_m_3_i(z, n, m, d)		pp_comma_if(n) m(z, n, d)

