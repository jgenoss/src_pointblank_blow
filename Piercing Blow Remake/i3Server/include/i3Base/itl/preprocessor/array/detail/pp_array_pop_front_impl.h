#pragma once


#define	pp_array_pop_front_impl(array)		pp_array_pop_front_z_impl(pp_deduce_z(), array)
#define pp_array_pop_front_z_impl(z, array) pp_array_pop_front_i(z, pp_array_size(array), array)

#define pp_array_pop_front_i(z, size, array) (pp_dec(size), (pp_enum_ ## z(pp_dec(size), pp_array_pop_front_m, array)))
#define pp_array_pop_front_m(z, n, data)	pp_array_elem(pp_inc(n), data)
