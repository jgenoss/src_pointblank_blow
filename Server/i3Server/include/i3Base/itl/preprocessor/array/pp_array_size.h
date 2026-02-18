#pragma once

#include "../tuple/pp_tuple_elem.h"
#include "detail/pp_array_size_impl.h"


#define pp_array_size(array)		pp_array_size_impl(array)
