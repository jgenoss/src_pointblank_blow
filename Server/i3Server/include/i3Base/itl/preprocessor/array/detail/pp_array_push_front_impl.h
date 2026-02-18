#pragma once

#define pp_array_push_front_impl(array, elem)	pp_array_push_front_i(pp_array_size(array), pp_array_data(array), elem)

#define pp_array_push_front_i(size, data, elem) (pp_inc(size), (elem pp_comma_if(size) pp_tuple_rem(size) data))
