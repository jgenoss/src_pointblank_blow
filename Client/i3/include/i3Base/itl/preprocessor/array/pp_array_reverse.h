#pragma once

#include "pp_array_data.h"
#include "pp_array_size.h"
#include "../tuple/pp_tuple_reverse.h"

#include "detail/pp_array_reverse_impl.h"

#define pp_array_reverse(array)		pp_array_reverse_impl(array)
