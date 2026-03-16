#pragma once

#include "../arithmetic/pp_inc.h"
#include "pp_array_data.h"
#include "pp_array_size.h"
#include "../punctuation/pp_comma_if.h"
#include "../tuple/pp_tuple_rem.h"

#include "detail/pp_array_push_back_impl.h"

#define pp_array_push_back(array, elem)		pp_array_push_back_impl(array, elem)
