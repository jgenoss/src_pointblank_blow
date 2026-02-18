#pragma once

#include "pp_array_data.h"		// pp_tuple_elemÆ÷ÇÔ..
#include "pp_array_size.h"

#include "detail/pp_array_elem_impl.h"

#define pp_array_elem(i, array)		pp_array_elem_impl(i, array)
