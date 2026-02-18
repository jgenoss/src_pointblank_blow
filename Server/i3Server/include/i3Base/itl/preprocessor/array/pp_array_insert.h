#pragma once

#include "pp_array_elem.h"				// pp_tuple_elem포함..
#include "pp_array_push_back.h"			// pp_inc, pp_array_size, pp_iif 포함..
#include "../comparison/pp_not_equal.h"
#include "../control/pp_deduce_d.h"		// pp_while포함..

#include "detail/pp_array_insert_impl.h"

#define pp_array_insert(array, i, elem)		pp_array_insert_impl(array, i, elem)
#define pp_array_insert_d(d, array, i, elem) pp_array_insert_d_impl(d, array, i, elem)

