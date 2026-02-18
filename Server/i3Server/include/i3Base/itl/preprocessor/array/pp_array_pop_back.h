#pragma once

#include "../arithmetic/pp_dec.h"
#include "pp_array_elem.h"			// pp_array_size Æ÷ÇÔ

#include "../repetition/pp_enum.h"
#include "../repetition/pp_deduce_z.h"

#include "detail/pp_array_pop_back_impl.h"

#define pp_array_pop_back(array)			pp_array_pop_back_impl(array)
#define pp_array_pop_back_z(z, array)		pp_array_pop_back_z_impl(z, array)

