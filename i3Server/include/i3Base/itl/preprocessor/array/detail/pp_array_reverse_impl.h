#pragma once

#define pp_array_reverse_impl(array) (pp_array_size(array), pp_tuple_reverse(pp_array_size(array), pp_array_data(array)))
