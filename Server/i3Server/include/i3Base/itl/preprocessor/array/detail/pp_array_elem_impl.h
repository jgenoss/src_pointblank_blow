#pragma once

#define pp_array_elem_impl(i, array) pp_tuple_elem(pp_array_size(array), i, pp_array_data(array))

