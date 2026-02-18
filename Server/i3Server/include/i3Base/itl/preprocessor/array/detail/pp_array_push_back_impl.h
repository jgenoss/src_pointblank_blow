#pragma once

#define pp_array_push_back_impl(array, elem) pp_array_push_back_i(pp_array_size(array), pp_array_data(array), elem)

#define pp_array_push_back_i(size, data, elem) (pp_inc(size), (pp_tuple_rem(size) data pp_comma_if(size) elem))

